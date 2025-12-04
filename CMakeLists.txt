cmake_minimum_required(VERSION 3.15)

project(ChildParentProcesses LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(runner STATIC
        lib/runner.cpp
)

target_include_directories(runner
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_executable(parent
        parent.cpp
)

target_link_libraries(parent
        PRIVATE runner
)

add_executable(child
        child.cpp
)
