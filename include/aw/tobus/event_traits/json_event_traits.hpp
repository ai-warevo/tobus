#ifndef AW_TOBUS_EVENT_TRAITS_JSON_EVENT_TRAITS_HPP
#define AW_TOBUS_EVENT_TRAITS_JSON_EVENT_TRAITS_HPP

/**
 * @file json_event_traits.hpp
 * @brief @c JsonEventTraits<T> — a ready-made @c EventTraits specialization that serialises @c T to JSON.
 *
 * Requires the type to provide @c to_json() → @c nlohmann::json and @c from_json(nlohmann::json) → void
 * (or @c static from_json(nlohmann::json) → @c T).
 *
 * The easiest way is to use @c NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE from nlohmann/json:
 *
 * @code
 * struct MyEvent { int id; std::string name; };
 * NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MyEvent, id, name)
 *
 * template <> struct aw::tobus::EventTraits<MyEvent>
 *     : aw::tobus::JsonEventTraits<MyEvent> {};
 * @endcode
 *
 * @note This header requires @c nlohmann/json. It is only available when the project is built with
 *       @c TOBUS_ENABLE_JSON=ON. Consumers that do not use @c JsonEventTraits are unaffected.
 */

#ifndef TOBUS_HAS_JSON
#error "json_event_traits.hpp requires TOBUS_HAS_JSON. Build with -DTOBUS_ENABLE_JSON=ON"
#endif

#include <aw/tobus/event_traits.hpp>

#include <nlohmann/json.hpp>

#include <string_view>
#include <vector>

namespace aw::tobus {

/**
 * @brief Ready-made @c EventTraits<T> that serialises @c T to/from JSON.
 *
 * The type must provide @c to_json() → @c nlohmann::json and either a static @c from_json(nlohmann::json) → @c T
 * or a non-static @c from_json(nlohmann::json) → void.
 * @tparam T Event type.
 */
template <typename T>
struct JsonEventTraits {
    /* Must be set by the user to a unique name for this event type. */
    static constexpr std::string_view name{};

    static std::vector<std::byte> serialize(const T& event) {
        auto j = event.to_json();
        auto text = j.dump();
        std::vector<std::byte> buf(text.size());
        for (std::size_t i = 0; i < text.size(); ++i)
            buf[i] = std::byte(static_cast<unsigned char>(text[i]));
        return buf;
    }

    static T deserialize(std::span<const std::byte> bytes) {
        std::string text;
        text.reserve(bytes.size());
        for (auto b : bytes)
            text.push_back(static_cast<char>(b));

        auto j = nlohmann::json::parse(text);

        constexpr bool has_static_from_json = requires {
            { T::from_json(j) } -> std::same_as<T>;
        };
        constexpr bool has_nonstatic_from_json = requires(T e) {
            { e.from_json(j) } -> std::same_as<void>;
        };

        if constexpr (has_static_from_json) {
            return T::from_json(j);
        } else if constexpr (has_nonstatic_from_json) {
            T event;
            event.from_json(j);
            return event;
        } else {
            static_assert(sizeof(T) == 0, "JsonEventTraits: T must provide to_json() and from_json()");
        }
    }
};

} // namespace aw::tobus

#endif  // AW_TOBUS_EVENT_TRAITS_JSON_EVENT_TRAITS_HPP
