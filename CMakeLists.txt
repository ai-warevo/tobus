cmake_minimum_required(VERSION 3.23)

include(FetchContent)
FetchContent_Declare(
    aw_cmake_modules
    GIT_REPOSITORY https://github.com/ai-warevo/template_cpp
    GIT_TAG main
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(aw_cmake_modules)
list(APPEND CMAKE_MODULE_PATH "${aw_cmake_modules_SOURCE_DIR}/cmake")

include(AWProject)
aw_project_prepare(NAME tobus NAMESPACE aw::tobus VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
project(${AW_PROJECT_NAME} VERSION ${AW_PROJECT_VERSION} LANGUAGES CXX)
aw_project_setup(PREFIX TOBUS BUILD_TESTS BUILD_EXAMPLES INSTALL)

find_package(Threads REQUIRED)
aw_check_cxx23_runtime()

include(DependencyCache)

option(TOBUS_ENABLE_UNIX_SOCKET "Build the UnixSocketBus stub" OFF)
option(TOBUS_ENABLE_TCP_SERVER "Build the TCPServerBus stub" OFF)
option(TOBUS_ENABLE_HTTP "Build the HTTPBus transport" OFF)
option(TOBUS_ENABLE_WEBSOCKET "Build the WebSocketBus stub" OFF)
option(TOBUS_ENABLE_REDIS "Enable the RedisBus stub (header-only)" OFF)
option(TOBUS_ENABLE_ZEROMQ "Enable the ZeroMQBus stub (header-only)" OFF)
option(TOBUS_ENABLE_RABBITMQ "Enable the RabbitMQBus stub (header-only)" OFF)
option(TOBUS_ENABLE_KAFKA "Enable the KafkaBus stub (header-only)" OFF)
option(TOBUS_ENABLE_JSON "Build with nlohmann/json support for JsonEventTraits" OFF)

if(NOT TARGET aw::tpl)
    aw_fetch_dependency(
        NAME aw_tpl
        GIT_REPOSITORY https://github.com/ai-warevo/tpl.git
        GIT_TAG v3.1.0
        OPTIONS
            AW_TPL_BUILD_TESTS OFF
            AW_TPL_BUILD_EXAMPLES OFF
            AW_TPL_INSTALL ${TOBUS_INSTALL})
endif()

if(TOBUS_ENABLE_JSON AND NOT TARGET nlohmann_json::nlohmann_json)
    aw_fetch_dependency(
        NAME nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        OPTIONS
            JSON_BuildTests OFF)
endif()

set(TOBUS_SOURCES
    src/errors.cpp
    src/transport_kind.cpp
    src/event_bus_factory.cpp
    src/detail/dispatcher.cpp
    src/detail/dispatcher.hpp
    src/transports/in_process_bus.cpp
    src/transports/framed_bus.cpp)

set(TOBUS_HEADERS
    include/aw/tobus/errors.hpp
    include/aw/tobus/event_bus.hpp
    include/aw/tobus/event_bus_config.hpp
    include/aw/tobus/event_bus_factory.hpp
    include/aw/tobus/event_handler.hpp
    include/aw/tobus/event_traits.hpp
    include/aw/tobus/detail/binary_event_traits_helpers.hpp
    include/aw/tobus/detail/event_traits_impl.hpp
    include/aw/tobus/event_traits/binary_event_traits.hpp
    include/aw/tobus/event_traits/json_event_traits.hpp
    include/aw/tobus/event_type_id.hpp
    include/aw/tobus/subscription.hpp
    include/aw/tobus/tobus.hpp
    include/aw/tobus/transport_kind.hpp
    include/aw/tobus/transports/framed_bus.hpp
    include/aw/tobus/transports/in_process_bus.hpp
    include/aw/tobus/transports/unix_socket_bus.hpp
    include/aw/tobus/transports/tcp_server_bus.hpp
    include/aw/tobus/transports/http_bus.hpp
    include/aw/tobus/transports/websocket_bus.hpp
    include/aw/tobus/transports/redis_bus.hpp
    include/aw/tobus/transports/zeromq_bus.hpp
    include/aw/tobus/transports/rabbitmq_bus.hpp
    include/aw/tobus/transports/kafka_bus.hpp)

if(TOBUS_ENABLE_UNIX_SOCKET)
    list(APPEND TOBUS_SOURCES src/transports/unix_socket_bus.cpp)
endif()
if(TOBUS_ENABLE_TCP_SERVER)
    list(APPEND TOBUS_SOURCES src/transports/tcp_server_bus.cpp)
endif()
if(TOBUS_ENABLE_HTTP)
    list(APPEND TOBUS_SOURCES src/transports/http_bus.cpp)
endif()
if(TOBUS_ENABLE_WEBSOCKET)
    list(APPEND TOBUS_SOURCES src/transports/websocket_bus.cpp)
endif()

aw_add_library(TARGET tobus TYPE STATIC ALIAS aw::tobus
    SOURCES ${TOBUS_SOURCES}
    HEADERS ${TOBUS_HEADERS}
    PUBLIC_INCLUDE_DIRS include
    PRIVATE_INCLUDE_DIRS src
    PUBLIC_LINKS aw::tpl Threads::Threads)

if(TOBUS_WARNINGS_AS_ERRORS)
    aw_configure_compiler(TARGET tobus STRICT_WARNINGS)
endif()

if(AW_ENABLE_COVERAGE)
    # Add coverage flags to library target for code coverage analysis
    target_compile_options(tobus PRIVATE --coverage)
    target_link_options(tobus PRIVATE --coverage)
endif()

set(_tobus_socket_transports UNIX_SOCKET TCP_SERVER HTTP WEBSOCKET)
set(_tobus_broker_transports REDIS ZEROMQ RABBITMQ KAFKA)
foreach(_transport IN LISTS _tobus_socket_transports _tobus_broker_transports)
    if(TOBUS_ENABLE_${_transport})
        target_compile_definitions(tobus PUBLIC TOBUS_HAS_${_transport}=1)
    else()
        target_compile_definitions(tobus PUBLIC TOBUS_HAS_${_transport}=0)
    endif()
endforeach()
if(TOBUS_ENABLE_JSON)
    target_compile_definitions(tobus PUBLIC TOBUS_HAS_JSON=1)
    target_link_libraries(tobus PUBLIC nlohmann_json::nlohmann_json)
endif()

if(TOBUS_INSTALL)
    aw_install_library(TARGET tobus NAME tobus NS aw::tobus DEPENDENCIES "tpl 3.1" Threads)
endif()

if(TOBUS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
aw_setup_tests(OPTION TOBUS_BUILD_TESTS DIRECTORY tests)
