cmake_minimum_required(VERSION 4.1)
project(chess_engine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES ./src/* ./src/*/* ./src/*/*/*)
add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Apps
add_executable(chess-gui apps/chess-gui/main.cpp)
target_link_libraries(chess-gui ${PROJECT_NAME})
