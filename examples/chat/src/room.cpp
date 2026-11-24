/* Room implementation — subscribes to UserJoined and MessageApproved. */

#include "room.hpp"

namespace aw::tobus::examples::chat {

Room::Room(aw::tobus::IEventBus& bus, void (*log)(std::string_view))
    : bus_(bus),
      log_(log),
      joined_(bus.subscribe<UserJoined>([this](const UserJoined& e) -> aw::tpl::threading::tasks::Task<void> {  // NOLINT
          log_("[room]      * " + e.name + " joined");
          co_return;
      })),
      approved_(bus.subscribe<MessageApproved>([this](const MessageApproved& e) -> aw::tpl::threading::tasks::Task<void> {  // NOLINT
          log_("[room]      " + e.from + ": " + e.text);
          co_return;
      })) {}

void Room::say(std::string from, std::string text) {
    bus_.publish(MessageSent{std::move(from), std::move(text)});
}

} // namespace aw::tobus::examples::chat
