/* Auth module — publishes UserJoined. Does not know who (if anyone) cares. */

#ifndef AW_TOBUS_CHAT_AUTH_HPP
#define AW_TOBUS_CHAT_AUTH_HPP

#include <aw/tobus/tobus.hpp>
#include "events.hpp"

namespace aw::tobus::examples::chat {

class Auth {
public:
    explicit Auth(aw::tobus::IEventBus& bus)
        : bus_(bus) {}

    void login(std::string name) { bus_.publish(UserJoined{std::move(name)}); }

private:
    aw::tobus::IEventBus& bus_;
};

} // namespace aw::tobus::examples::chat

#endif  // AW_TOBUS_CHAT_AUTH_HPP
