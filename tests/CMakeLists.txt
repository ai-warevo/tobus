include(Tests)
if(TOBUS_WARNINGS_AS_ERRORS)
    aw_test_target(NAME tobus
        SOURCES
            test_event_types.cpp
            test_errors_coverage.cpp
            test_in_process_bus.cpp
            test_subscription.cpp
            test_factory.cpp
            test_stubs.cpp
            test_http_bus.cpp
            test_event_traits.cpp
            test_framed_bus_coverage.cpp
        LINK_TARGET aw::tobus
        STRICT_WARNINGS)
else()
    aw_test_target(NAME tobus
        SOURCES
            test_event_types.cpp
            test_errors_coverage.cpp
            test_in_process_bus.cpp
            test_subscription.cpp
            test_factory.cpp
            test_stubs.cpp
            test_http_bus.cpp
            test_event_traits.cpp
            test_framed_bus_coverage.cpp
        LINK_TARGET aw::tobus)
endif()

# The cases tagged [threads] (concurrent publishers, subscribe/unsubscribe races, ...) as one named CTest entry, so CI
# can hammer just them: ctest -R tobus_concurrency --repeat until-fail:30
add_test(NAME tobus_concurrency COMMAND tobus_tests "[threads]")
set_tests_properties(tobus_concurrency PROPERTIES LABELS "concurrency" TIMEOUT 120)
