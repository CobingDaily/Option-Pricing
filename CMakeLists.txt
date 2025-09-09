cmake_minimum_required(VERSION 4.0)
project(Option_Pricing)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(Option_Pricing
        src/main.cpp)
