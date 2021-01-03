cmake_minimum_required(VERSION 3.10)
project(backtest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)

add_executable(backtest.exe backtest.cpp)

target_link_libraries(backtest.exe PUBLIC backtest)
target_include_directories(backtest.exe PUBLIC "${PROJECT_SOURCE_DIR}/src")

enable_testing()
find_package(GTest REQUIRED)

add_subdirectory(unit_tests)

