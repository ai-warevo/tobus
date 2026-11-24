#ifndef AW_TOBUS_EVENT_TRAITS_BINARY_EVENT_TRAITS_HPP
#define AW_TOBUS_EVENT_TRAITS_BINARY_EVENT_TRAITS_HPP

/**
 * @file binary_event_traits.hpp
 * @brief BinaryEventTraits<T> — a ready-made specialization of EventTraits<T> that serializes T into raw bytes.
 *
 * Two modes are supported:
 *
 * 1. Trivial mode (default). Any trivially-copyable type of size <= 256 bytes is copied
 *    via std::memcpy. Format: one length byte + raw bytes.
 *
 * 2. Field mode. For non-trivial types (e.g., containing std::string), specialize
 *    to_binary() and from_binary():
 *
 *    to_binary returns std::vector<std::byte>, from_binary takes span<const std::byte> and T&.
 *
 * The serialized payload is prefixed with one length byte (max. 255).
 * BinaryEventTraits is the foundation for hex-encoded HTTP transport: the transport hex-encodes
 * raw bytes; the traits themselves produce and consume only raw bytes.
 */

#include <aw/tobus/event_traits.hpp>

#include <aw/tobus/detail/binary_event_traits_helpers.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

namespace aw::tobus {

/**
 * @brief A ready-made specialization of EventTraits<T> that serializes T into raw bytes.
 *
 * For trivially-copyable types <= 255 bytes, the payload is one length byte + std::memcpy of the object.
 * For complex types, the user can specialize to_binary() / from_binary().
 * @tparam T Event type.
 */
template <typename T>
struct BinaryEventTraits {
     /* Must be set by the user to a unique name for this event type. */
     static constexpr std::string_view name{};

    static std::vector<std::byte> serialize(const T& event) {
        constexpr std::size_t MaxSize = 255;
        constexpr bool is_trivial = std::is_trivially_copyable_v<T>;
        constexpr bool has_custom = requires(const T& e) {
            { EventTraits<T>::to_binary(e) } -> std::convertible_to<std::vector<std::byte>>;
        };

        if constexpr (has_custom) {
            auto payload = EventTraits<T>::to_binary(event);
            if (payload.size() > MaxSize) throw std::runtime_error("BinaryEventTraits: payload exceeds 255 bytes");
            std::vector<std::byte> buf;
            buf.reserve(1 + payload.size());
            buf.push_back(std::byte(payload.size()));
            buf.insert(buf.end(), payload.begin(), payload.end());
            return buf;
        } else if constexpr (is_trivial) {
            static_assert(sizeof(T) <= MaxSize, "BinaryEventTraits: trivially-copyable type exceeds 255 bytes");
            std::vector<std::byte> buf(1 + sizeof(T));
            buf[0] = std::byte(sizeof(T));
            std::memcpy(buf.data() + 1, &event, sizeof(T));
            return buf;
        } else {
            static_assert(!is_trivial && !has_custom,
                          "BinaryEventTraits: T is neither trivially-copyable nor has a custom to_binary()");
        }
    }

    static T deserialize(std::span<const std::byte> bytes) {
        if (bytes.empty() || bytes[0] == std::byte(0))
            throw std::runtime_error("BinaryEventTraits: zero-length payload");

        auto payload_size = static_cast<std::size_t>(bytes[0]);
        if (payload_size + 1 > bytes.size()) throw std::runtime_error("BinaryEventTraits: payload size exceeds buffer");

        std::span<const std::byte> payload = bytes.subspan(1, payload_size);

        constexpr bool has_custom = requires(T e, std::span<const std::byte> b) {
            { EventTraits<T>::from_binary(b, e) } -> std::same_as<void>;
        };
        constexpr bool is_trivial = std::is_trivially_copyable_v<T>;

        if constexpr (has_custom) {
            T event{};
            EventTraits<T>::from_binary(payload, event);
            return event;
        } else if constexpr (is_trivial) {
            T event{};
            std::memcpy(&event, payload.data(), payload_size);
            return event;
        } else {
            static_assert(is_trivial || has_custom,
                          "BinaryEventTraits: T is neither trivially-copyable nor has a custom from_binary()");
        }
    }
};

} // namespace aw::tobus

#endif  // AW_TOBUS_EVENT_TRAITS_BINARY_EVENT_TRAITS_HPP
