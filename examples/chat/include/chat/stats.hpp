/* Stats module — pure observer: subscribes to everything, publishes nothing. */
/* Adding it required no change in any other module. */

#ifndef AW_TOBUS_CHAT_STATS_HPP
#define AW_TOBUS_CHAT_STATS_HPP

#include <atomic>
#include <aw/tobus/tobus.hpp>
#include "events.hpp"

namespace aw::tobus::examples::chat {

struct StatsData {
    /* Handlers run concurrently on different threads, hence atomics. */
    std::atomic<int> joined{0};
    std::atomic<int> sent{0};
    std::atomic<int> approved{0};
    std::atomic<int> blocked{0};
};

class Stats {
public:
    explicit Stats(aw::tobus::IEventBus& bus);

    StatsData data;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

private:
    aw::tobus::Subscription joined_;
    aw::tobus::Subscription sent_;
    aw::tobus::Subscription approved_;
    aw::tobus::Subscription blocked_;
};

} // namespace aw::tobus::examples::chat

#endif  // AW_TOBUS_CHAT_STATS_HPP
