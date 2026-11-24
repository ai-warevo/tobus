/* Moderation implementation — reacts to MessageSent with a cascade (publishes Approved or Blocked). */

#include "moderation.hpp"
#include <aw/tpl/threading/tasks/async_sleep.hpp>

#include <chrono>

using namespace std::chrono_literals;

namespace aw::tobus::examples::chat {

Moderation::Moderation(aw::tobus::IEventBus& bus)
    : bus_(bus),
      sub_(bus.subscribe<MessageSent>([this](const MessageSent& e) -> aw::tpl::threading::tasks::Task<void> {  // NOLINT
          /*
           * Pretend the check is slow (a remote spam filter, say).
           * co_await suspends only this handler; the publisher and others keep running.
           */
          co_await aw::tpl::threading::tasks::async_sleep(20ms);

          if (e.text.find("spam") != std::string::npos) {
              bus_.publish(MessageBlocked{e.from, "looks like spam"}); // cascade
          } else {
              bus_.publish(MessageApproved{e.from, e.text});
          }
      })) {}

} // namespace aw::tobus::examples::chat
