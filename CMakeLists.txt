cmake_minimum_required(VERSION 4.1)
project(chess_engine)

set(CMAKE_CXX_STANDARD 20)

# Apps
add_executable(chess-gui apps/chess-gui/main.cpp)
