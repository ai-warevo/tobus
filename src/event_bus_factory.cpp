#include <aw/tobus/event_bus_factory.hpp>

#include <aw/tobus/errors.hpp>
#include <aw/tobus/transports/in_process_bus.hpp>

#if TOBUS_HAS_UNIX_SOCKET
#include <aw/tobus/transports/unix_socket_bus.hpp>
#endif
#if TOBUS_HAS_TCP_SERVER
#include <aw/tobus/transports/tcp_server_bus.hpp>
#endif
#if TOBUS_HAS_HTTP
#include <aw/tobus/transports/http_bus.hpp>
#endif
#if TOBUS_HAS_WEBSOCKET
#include <aw/tobus/transports/websocket_bus.hpp>
#endif
#if TOBUS_HAS_REDIS
#include <aw/tobus/transports/redis_bus.hpp>
#endif
#if TOBUS_HAS_ZEROMQ
#include <aw/tobus/transports/zeromq_bus.hpp>
#endif
#if TOBUS_HAS_RABBITMQ
#include <aw/tobus/transports/rabbitmq_bus.hpp>
#endif
#if TOBUS_HAS_KAFKA
#include <aw/tobus/transports/kafka_bus.hpp>
#endif

namespace aw::tobus {

bool EventBusFactory::is_available(TransportKind kind) noexcept {
    /* Each branch reads a different compile-time TOBUS_HAS_* switch. */
    switch (kind) {
    case TransportKind::in_process:
        return true;
    case TransportKind::unix_socket:
        return TOBUS_HAS_UNIX_SOCKET != 0;
    case TransportKind::tcp_server:
        return TOBUS_HAS_TCP_SERVER != 0;
    case TransportKind::http:
        return TOBUS_HAS_HTTP != 0;
    case TransportKind::websocket:
        return TOBUS_HAS_WEBSOCKET != 0;
    case TransportKind::redis:
        return TOBUS_HAS_REDIS != 0;
    case TransportKind::zeromq:
        return TOBUS_HAS_ZEROMQ != 0;
    case TransportKind::rabbitmq:
        return TOBUS_HAS_RABBITMQ != 0;
    case TransportKind::kafka:
        return TOBUS_HAS_KAFKA != 0;
    }
    return false;
}

std::shared_ptr<IEventBus> EventBusFactory::create(const EventBusConfig& config) {
    if (!is_available(config.transport)) {
        throw TransportNotAvailable(config.transport);
    }
    switch (config.transport) {
    case TransportKind::in_process:
        return std::make_shared<InProcessBus>(config);
#if TOBUS_HAS_UNIX_SOCKET
    case TransportKind::unix_socket:
        return std::make_shared<UnixSocketBus>(config);
#endif
#if TOBUS_HAS_TCP_SERVER
    case TransportKind::tcp_server:
        return std::make_shared<TCPServerBus>(config);
#endif
#if TOBUS_HAS_HTTP
    case TransportKind::http:
        return std::make_shared<HTTPBus>(config);
#endif
#if TOBUS_HAS_WEBSOCKET
    case TransportKind::websocket:
        return std::make_shared<WebSocketBus>(config);
#endif
#if TOBUS_HAS_REDIS
    case TransportKind::redis:
        return std::make_shared<RedisBus>(config);
#endif
#if TOBUS_HAS_ZEROMQ
    case TransportKind::zeromq:
        return std::make_shared<ZeroMQBus>(config);
#endif
#if TOBUS_HAS_RABBITMQ
    case TransportKind::rabbitmq:
        return std::make_shared<RabbitMQBus>(config);
#endif
#if TOBUS_HAS_KAFKA
    case TransportKind::kafka:
        return std::make_shared<KafkaBus>(config);
#endif
    default:
        break;
    }
    throw TransportNotAvailable(config.transport);
}

} // namespace aw::tobus
