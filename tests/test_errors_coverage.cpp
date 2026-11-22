#include <catch2/catch_test_macros.hpp>

#include <aw/tobus/errors.hpp>
#include <aw/tobus/tobus.hpp>

using namespace aw::tobus;

TEST_CASE("TransportNotAvailable exception messages include CMake option names", "[errors][coverage]") {
    // Test that all transport kinds generate proper CMake option names
    // Format should be "TOBUS_ENABLE_<UPPERCASE_NAME>"

    SECTION("in_process") {
        auto msg = std::string(TransportNotAvailable(TransportKind::in_process).what());
        REQUIRE(msg.find("TOBUS_ENABLE_IN_PROCESS") != std::string::npos);
    }

    SECTION("unix_socket") {
        auto msg = std::string(TransportNotAvailable(TransportKind::unix_socket).what());
        REQUIRE(msg.find("TOBUS_ENABLE_UNIX_SOCKET") != std::string::npos);
    }

    SECTION("tcp_server") {
        auto msg = std::string(TransportNotAvailable(TransportKind::tcp_server).what());
        REQUIRE(msg.find("TOBUS_ENABLE_TCP_SERVER") != std::string::npos);
    }

    SECTION("http") {
        auto msg = std::string(TransportNotAvailable(TransportKind::http).what());
        REQUIRE(msg.find("TOBUS_ENABLE_HTTP") != std::string::npos);
    }

    SECTION("websocket") {
        auto msg = std::string(TransportNotAvailable(TransportKind::websocket).what());
        REQUIRE(msg.find("TOBUS_ENABLE_WEBSOCKET") != std::string::npos);
    }

    SECTION("redis") {
        auto msg = std::string(TransportNotAvailable(TransportKind::redis).what());
        REQUIRE(msg.find("TOBUS_ENABLE_REDIS") != std::string::npos);
    }

    SECTION("zeromq") {
        auto msg = std::string(TransportNotAvailable(TransportKind::zeromq).what());
        REQUIRE(msg.find("TOBUS_ENABLE_ZEROMQ") != std::string::npos);
    }

    SECTION("rabbitmq") {
        auto msg = std::string(TransportNotAvailable(TransportKind::rabbitmq).what());
        REQUIRE(msg.find("TOBUS_ENABLE_RABBITMQ") != std::string::npos);
    }

    SECTION("kafka") {
        auto msg = std::string(TransportNotAvailable(TransportKind::kafka).what());
        REQUIRE(msg.find("TOBUS_ENABLE_KAFKA") != std::string::npos);
    }
}

TEST_CASE("NotSerializable exception preserves event type name", "[errors][coverage]") {
    SECTION("simple name") {
        auto ex = NotSerializable("MyEvent");
        auto msg = std::string(ex.what());
        REQUIRE(msg.find("MyEvent") != std::string::npos);
        REQUIRE(msg.find("cannot be sent over this transport") != std::string::npos);
        REQUIRE(msg.find("EventTraits") != std::string::npos);
    }

    SECTION("qualified name") {
        auto ex = NotSerializable("my.namespace.EventType");
        auto msg = std::string(ex.what());
        REQUIRE(msg.find("my.namespace.EventType") != std::string::npos);
    }

    SECTION("empty name") {
        auto ex = NotSerializable("");
        auto msg = std::string(ex.what());
        REQUIRE(msg.find("cannot be sent over this transport") != std::string::npos);
    }
}

TEST_CASE("TransportNotImplemented exception preserves operation name", "[errors][coverage]") {
    SECTION("operation name") {
        auto ex = TransportNotImplemented("HTTP POST");
        auto msg = std::string(ex.what());
        REQUIRE(msg.find("HTTP POST") != std::string::npos);
        REQUIRE(msg.find("not implemented yet") != std::string::npos);
    }

    SECTION("empty operation") {
        auto ex = TransportNotImplemented("");
        auto msg = std::string(ex.what());
        REQUIRE(msg.find("not implemented yet") != std::string::npos);
    }
}

TEST_CASE("TobusError is a std::runtime_error", "[errors][coverage]") {
    TobusError err("test error");
    REQUIRE(std::string(err.what()) == "test error");

    // Verify it's a std::runtime_error
    std::runtime_error& as_runtime = err;
    (void)as_runtime;

    REQUIRE(true);
}

TEST_CASE("All exception types inherit from TobusError", "[errors][coverage]") {
    // TransportNotAvailable
    TransportNotAvailable nav(TransportKind::redis);
    const TobusError& nav_err = nav;
    REQUIRE(std::string(nav_err.what()).find("TOBUS_ENABLE_REDIS") != std::string::npos);

    // TransportNotImplemented
    TransportNotImplemented tni("test op");
    const TobusError& tni_err = tni;
    REQUIRE(std::string(tni_err.what()).find("test op") != std::string::npos);

    // NotSerializable
    NotSerializable ns("TestType");
    const TobusError& ns_err = ns;
    REQUIRE(std::string(ns_err.what()).find("TestType") != std::string::npos);
}
