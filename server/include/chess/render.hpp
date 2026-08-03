#pragma once

#include <string>
#include <chess/board.hpp>
#include <chess/piece.hpp>


namespace chess {
    class Render {
        static std::string get_piece_symbol(const Piece& piece);

    public:
        static void draw_board(const Board& board);
    };
}
