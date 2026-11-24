/* Notifier implementation. */

#include "notifier.hpp"

namespace aw::tobus::examples::chat {

Notifier::Notifier(aw::tobus::IEventBus& bus, void (*log)(std::string_view))
    : sub_(bus.subscribe<MessageBlocked>([log](const MessageBlocked& e) -> aw::tpl::threading::tasks::Task<void> {  // NOLINT
          log("[notifier]  @" + e.from + ": your message was blocked (" + e.reason + ")");
          co_return;
      })) {}

} // namespace aw::tobus::examples::chat
