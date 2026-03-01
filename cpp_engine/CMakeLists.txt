cmake_minimum_required(VERSION 3.20)
project(execution_intelligence_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(yaml-cpp REQUIRED)

add_executable(engine
    src/main.cpp
    src/config.cpp
    src/jsonl_logger.cpp
    src/time_utils.cpp
    src/session.cpp
)

target_include_directories(engine PRIVATE include)
target_link_libraries(engine PRIVATE yaml-cpp::yaml-cpp)

# Warnings
if (MSVC)
  target_compile_options(engine PRIVATE /W4)
else()
  target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)
endif()