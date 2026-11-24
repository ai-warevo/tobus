/* Event types shared by all chat modules. */
/* Each struct is the whole "contract" between modules — no base class, no framework. */

#ifndef AW_TOBUS_CHAT_EVENTS_HPP
#define AW_TOBUS_CHAT_EVENTS_HPP

#include <string>

namespace aw::tobus::examples::chat {

struct UserJoined {
    std::string name;
};

struct MessageSent { /* raw input from a user, not yet checked */
    std::string from;
    std::string text;
};

struct MessageApproved { /* moderation said "ok": safe to show to everybody */
    std::string from;
    std::string text;
};

struct MessageBlocked { /* moderation said "no" */
    std::string from;
    std::string reason;
};

} // namespace aw::tobus::examples::chat

#endif  // AW_TOBUS_CHAT_EVENTS_HPP
