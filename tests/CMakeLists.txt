# tests/CMakeLists.txt

enable_testing()

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Add test executables
add_executable(message_interface_test
    unit/test_message_interface.cpp
)

add_executable(arinc429_message_test
    unit/arinc429/test_arinc429_message.cpp
)

add_executable(canj1939_message_test
    unit/canj1939/test_canj1939_message.cpp
)

add_executable(generator_interface_test
    unit/test_generator_interface.cpp
)

add_executable(arinc429_generator_test
    unit/arinc429/test_arinc429_generator.cpp
)

add_executable(canj1939_generator_test
    unit/canj1939/test_canj1939_generator.cpp
)

# Common test configuration
function(configure_test TEST_NAME)
    target_link_libraries(${TEST_NAME}
        PRIVATE
            serial_bus_generator
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
            GTest::gmock_main
            Threads::Threads
    )
    
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
    )
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Configure all tests
configure_test(message_interface_test)
configure_test(arinc429_message_test)
configure_test(canj1939_message_test)
configure_test(generator_interface_test)
configure_test(arinc429_generator_test)
configure_test(canj1939_generator_test)