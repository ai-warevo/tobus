/* Moderation module — checks every MessageSent and answers with MessageApproved or MessageBlocked. */

#ifndef AW_TOBUS_CHAT_MODERATION_HPP
#define AW_TOBUS_CHAT_MODERATION_HPP

#include <aw/tobus/tobus.hpp>
#include "events.hpp"

namespace aw::tobus::examples::chat {

class Moderation {
public:
    explicit Moderation(aw::tobus::IEventBus& bus);

private:
    aw::tobus::IEventBus& bus_;
    aw::tobus::Subscription sub_;
};

} // namespace aw::tobus::examples::chat

#endif  // AW_TOBUS_CHAT_MODERATION_HPP
