# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(serial_bus_generator VERSION 0.1.0 LANGUAGES CXX)

find_package(Threads REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTING "Build the testing tree." ON)

# Add compile options
add_compile_options(-Wall -Wextra -Wpedantic)
add_library(serial_bus_generator
    src/core/data_generator.cpp
    src/protocols/arinc429/arinc429_message.cpp
    src/protocols/arinc429/arinc429_generator.cpp
    src/protocols/canj1939/canj1939_message.cpp
    src/protocols/canj1939/canj1939_generator.cpp
)
# Set include directories for the library
target_include_directories(serial_bus_generator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_subdirectory(src)
option(BUILD_TESTING "Build the testing tree." OFF)

# Only try to find GTest if building tests
if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()