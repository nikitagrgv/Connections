cmake_minimum_required(VERSION 3.24)
project(signals)

set(CMAKE_CXX_STANDARD 17)

add_executable(signals main.cpp Callable.h Signal.h AutoSignal.h Object.h Object.cpp)
