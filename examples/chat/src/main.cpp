/*
 * Chat example — composition root.
 *
 * This is the only file that knows which transport is used. Change `config.transport`
 * to move the same modules onto another one (a socket, a broker); the modules below stay untouched.
 */

#include <aw/tobus/tobus.hpp>
#include <aw/tpl/threading/tasks/async_sleep.hpp>

#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "auth.hpp"
#include "moderation.hpp"
#include "notifier.hpp"
#include "room.hpp"
#include "stats.hpp"

using namespace std::chrono_literals;

/* ---- helpers ---- */

namespace {

/* Serialize console output (handlers run on pool threads). */
void log(std::string_view line) {
    static std::mutex m;
    std::lock_guard lock(m);
    std::cout << "[thread " << std::this_thread::get_id() << "] " << line << "\n";
}

}  // namespace

/* ---- main ---- */

int main() {
    aw::tobus::EventBusConfig config;
    config.transport = aw::tobus::TransportKind::in_process;
    /* An exception thrown in any handler ends up here, never at the publisher. */
    config.on_error = [](const std::exception_ptr& ep) {
        try {
            std::rethrow_exception(ep);
        } catch (const std::exception& ex) {
            log(std::string("[error]     handler failed: ") + ex.what());
        }
    };

    auto bus = aw::tobus::EventBusFactory::create(config);
    log("transport: " + std::string(bus->transport_name()));

    /* Compose modules — each one only knows IEventBus&. */
    aw::tobus::examples::chat::Auth auth(*bus);
    aw::tobus::examples::chat::Room room(*bus, log);
    aw::tobus::examples::chat::Moderation moderation(*bus);
    aw::tobus::examples::chat::Notifier notifier(*bus, log);
    aw::tobus::examples::chat::Stats stats(*bus);

    /*
     * Two "users" act. Every call returns immediately (fire-and-forget):
     * nobody waits for moderation, the room, or the stats. They happen in
     * the background, on the thread pool.
     */
    auth.login("alice");
    auth.login("bob");
    room.say("alice", "hello everyone!");
    room.say("bob", "buy cheap spam now");
    room.say("alice", "welcome, bob");

    /*
     * drain() completes once every started delivery has finished, including
     * the ones triggered by other handlers (Moderation's cascade).
     * Use it at shutdown or in tests; normal operation never needs to wait.
     */
    bus->drain().wait();

    std::cout << "\njoined=" << stats.data.joined << " sent=" << stats.data.sent << " approved=" << stats.data.approved
              << " blocked=" << stats.data.blocked << "\n";

    return 0;
}
