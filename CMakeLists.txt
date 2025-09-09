cmake_minimum_required(VERSION 4.0)
project(Option_Pricing VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED, ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(option_lib
        src/option.cpp
)

add_executable(pricer
        src/main.cpp
)

target_link_libraries(pricer option_lib)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(pricer PRIVATE -Wall -Wextra -Wpedantic)
endif()