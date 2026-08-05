cmake_minimum_required(VERSION 4.1)
project(chess_engine)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-38-2
)
FetchContent_MakeAvailable(asio)

find_package(Threads REQUIRED)
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
target_link_libraries(asio INTERFACE Threads::Threads)

file(GLOB SOURCES
    ./src/*
    ./src/*/*
    ./src/*/*/*
)
add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        asio
)

# Apps
add_executable(chess-gui apps/chess-gui/main.cpp)
target_link_libraries(chess-gui ${PROJECT_NAME})

# Tests
add_executable(tcp_test tests/tcp_test.cpp)
target_link_libraries(tcp_test ${PROJECT_NAME})
