cmake_minimum_required(VERSION 4.0)
project(Option_Pricing VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Build type not specified: defaulting to Release")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(pricer_lib
        src/option.cpp
        src/monte_carlo.cpp
        src/black_scholes.cpp
)

add_executable(pricer
        src/main.cpp
)

target_link_libraries(pricer pricer_lib)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(pricer PRIVATE
            -Wall -Wextra -Wpedantic
            $<$<CONFIG:Release>:-O3 -march=native -DNDEBUG>
            $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
    )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")