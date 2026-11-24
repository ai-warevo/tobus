/* Notifier module — tells the author their message was blocked. */
/* Knows nothing about Moderation, only about MessageBlocked. */

#ifndef AW_TOBUS_CHAT_NOTIFIER_HPP
#define AW_TOBUS_CHAT_NOTIFIER_HPP

#include <aw/tobus/tobus.hpp>
#include "events.hpp"

namespace aw::tobus::examples::chat {

class Notifier {
public:
    explicit Notifier(aw::tobus::IEventBus& bus, void (*log)(std::string_view));

private:
    aw::tobus::Subscription sub_;
};

} // namespace aw::tobus::examples::chat

#endif  // AW_TOBUS_CHAT_NOTIFIER_HPP
