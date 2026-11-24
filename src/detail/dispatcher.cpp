#include "detail/dispatcher.hpp"

#include <aw/tpl/threading/tasks/spawn.hpp>

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace aw::tobus::detail {

namespace {

/*
 * What happens to a failure when the application gave no EventBusConfig::on_error: one line on stderr, so that a
 * broken handler never disappears without a trace. Applications that really want silence install a no-op handler.
 */
void write_default_report(std::string_view event_name, const std::exception_ptr& ep) noexcept {
    try {
        std::string line = "[tobus] handler for event '" + std::string(event_name) + "' failed: ";
        try {
            std::rethrow_exception(ep);
        } catch (const std::exception& e) {
            line += e.what();
        } catch (...) {
            line += "unknown exception";
        }
        line += '\n';
        std::cerr << line << std::flush; /* one insertion: lines from concurrent handlers do not interleave mid-line */
    } catch (...) {
    }
}

} // namespace

Dispatcher::Dispatcher(PrivateTag, ErrorHandler on_error)
    : on_error_(std::move(on_error)) {}

std::shared_ptr<Dispatcher> Dispatcher::create(ErrorHandler on_error) {
    return std::make_shared<Dispatcher>(PrivateTag{}, std::move(on_error));
}

Subscription Dispatcher::subscribe(const EventTypeInfo& type, RawEventHandler handler) {
    auto shared_handler = std::make_shared<RawEventHandler>(std::move(handler));
    std::uint64_t token = 0;
    {
        std::lock_guard lock(mtx_);
        const auto [it, inserted] = identities_.try_emplace(type.id, type.type);
        /* Hash collision: two different C++ types sharing this id would receive each other's events. */
        if (!inserted && *it->second != *type.type) {
            throw std::logic_error(collision_message(type.name));
        }
        token = next_token_++;
        auto next = std::make_shared<Table>(*table_);
        (*next)[type.id].push_back(Entry{token, std::move(shared_handler)});
        table_ = std::move(next);
    }
    return Subscription([weak = weak_from_this(), id = type.id, token] {
        if (auto self = weak.lock()) {
            self->unsubscribe(id, token);
        }
    });
}

void Dispatcher::unsubscribe(EventTypeId id, std::uint64_t token) noexcept {
    try {
        std::lock_guard lock(mtx_);
        auto next = std::make_shared<Table>(*table_);
        const auto it = next->find(id);
        if (it == next->end()) {
            return;
        }
        std::erase_if(it->second, [token](const Entry& e) { return e.token == token; });
        if (it->second.empty()) {
            next->erase(it);
        }
        table_ = std::move(next);
    } catch (...) {
    }
}

std::size_t Dispatcher::subscriber_count(EventTypeId id) const {
    std::shared_ptr<const Table> snapshot;
    {
        std::lock_guard lock(mtx_);
        snapshot = table_;
    }
    const auto it = snapshot->find(id);
    return it == snapshot->end() ? 0 : it->second.size();
}

void Dispatcher::dispatch(const EventTypeInfo& type, const std::shared_ptr<const void>& event) {
    std::shared_ptr<const Table> snapshot;
    {
        std::lock_guard lock(mtx_);
        /* Type safety: handlers reinterpret the payload as their own type. If another type shares this id, delivering would be undefined behaviour, so refuse instead. */
        const auto known = identities_.find(type.id);
        if (known != identities_.end() && *known->second != *type.type) {
            throw std::logic_error(collision_message(type.name));
        }
        snapshot = table_;
    }
    const auto it = snapshot->find(type.id);
    if (it == snapshot->end()) {
        return;
    }
    auto self = shared_from_this();
    for (const Entry& entry : it->second) {
        /* Count before starting: run() may finish (and call done()) on another thread immediately. */
        inflight_.add();
        try {
            aw::tpl::threading::tasks::co_spawn(run(self, entry.handler, event, type.name));
        } catch (...) {
            inflight_.done();
            report(std::current_exception(), type.name);
        }
    }
}

/* Lifetime safety: the frame owns `self`, the handler and the event until the end: the handler's `const T&` can never dangle, and the group outlives the final done(). */
Task<void> Dispatcher::run(std::shared_ptr<Dispatcher> self, std::shared_ptr<RawEventHandler> handler,
                           std::shared_ptr<const void> event, std::string_view event_name) {
    try {
        co_await (*handler)(event.get());
    } catch (...) {
        self->report(std::current_exception(), event_name);
    }
    handler.reset();
    event.reset();
    self->inflight_.done();
}

Task<void> Dispatcher::drain(std::shared_ptr<Dispatcher> self) {
    co_await self->inflight_.wait_async();
}

std::string Dispatcher::collision_message(std::string_view event_name) {
    return "tobus: event type id collision for '" + std::string(event_name) +
           "': another C++ type already uses this EventTraits<T>::name; give each event type a unique name";
}

void Dispatcher::report(const std::exception_ptr& ep, std::string_view event_name) noexcept {
    if (!on_error_) {
        write_default_report(event_name, ep);
        return;
    }
    try {
        on_error_(ep);
    } catch (...) {
    }
}

} // namespace aw::tobus::detail
