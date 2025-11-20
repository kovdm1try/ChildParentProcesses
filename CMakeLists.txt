set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
        runner STATIC
        lib/runner.cpp
)

target_include_directories(
        runner
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_executable(parent parent.cpp)
add_executable(child child.cpp)