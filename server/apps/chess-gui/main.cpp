#include <iostream>
#include <string>
#include <chess/board.hpp>
#include <chess/render.hpp>
#include <chess/move.hpp>


int main() {
    chess::Board board = chess::Board::create_default();
    chess::Render::draw_board(board);

    std::string input;
    std::cin >> input;
    chess::Move m = chess::Move::parse(input);
}
