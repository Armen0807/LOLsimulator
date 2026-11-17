cmake_minimum_required(VERSION 4.4)
project(LOLsimulator)

set(CMAKE_CXX_STANDARD 20)

add_library(engine src/engine.cpp)
target_include_directories(engine PUBLIC src)

add_executable(LOLsimulator src/main.cpp)
target_link_libraries(LOLsimulator engine)

add_executable(tests tests/test_engine.cpp)
target_link_libraries(tests engine)