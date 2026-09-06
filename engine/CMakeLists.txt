cmake_minimum_required(VERSION 3.5)
project(chess_engine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# main exec
add_executable(mockfish main.cpp)
target_link_libraries(mockfish ${PROJECT_NAME})
set_target_properties(mockfish
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# Tests
add_executable(board_test tests/board_test.cpp)
target_link_libraries(board_test ${PROJECT_NAME})

add_executable(game_test tests/game_test.cpp)
target_link_libraries(game_test ${PROJECT_NAME})

add_executable(move_generator_test tests/move_generator_test.cpp)
target_link_libraries(move_generator_test ${PROJECT_NAME})
