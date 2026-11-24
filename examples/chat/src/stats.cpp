/* Stats implementation. */

#include "stats.hpp"

namespace aw::tobus::examples::chat {

Stats::Stats(aw::tobus::IEventBus& bus)
    : joined_(bus.subscribe<UserJoined>([this](const UserJoined&) -> aw::tpl::threading::tasks::Task<void> {  // NOLINT
          ++data.joined;
          co_return;
      })),
      sent_(bus.subscribe<MessageSent>([this](const MessageSent&) -> aw::tpl::threading::tasks::Task<void> {  // NOLINT
          ++data.sent;
          co_return;
      })),
      approved_(bus.subscribe<MessageApproved>([this](const MessageApproved&) -> aw::tpl::threading::tasks::Task<void> {  // NOLINT
          ++data.approved;
          co_return;
      })),
      blocked_(bus.subscribe<MessageBlocked>([this](const MessageBlocked&) -> aw::tpl::threading::tasks::Task<void> {  // NOLINT
          ++data.blocked;
          co_return;
      })) {}

} // namespace aw::tobus::examples::chat
