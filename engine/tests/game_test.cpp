#include <chess/game.hpp>
#include <iostream>
#include <bitset>


constexpr uint8_t square(const char col, const int row) {
    return (8 - row) * 8 + (col - 'a');
}


uint8_t parse(const std::string &s) {
    if (s.length() != 2) {
        throw std::invalid_argument("Algebraic notation must be given");
    }
    const uint8_t x = s[0] - 'a';
    const uint8_t y = 7 - (s[1] - '1');
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        throw std::invalid_argument("Algebraic notation must be given");
    }
    return y * 8 + x;
}


int main() {
    using namespace mf::chess;

    Game game{};
    draw_board(game.get_current_board());
    // game.make_move({parse("e2"), parse("e3"), WHITE_PAWN, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("f1"), parse("e2"), WHITE_BISHOP, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("g1"), parse("f3"), WHITE_KNIGHT, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("e1"), parse("g1"), WHITE_KING, NONE, NONE, CASTLE});

    game.make_move({parse("h2"), parse("h3"), WHITE_PAWN, NONE, NONE, static_cast<MoveType>(0)});
    game.make_move({parse("h7"), parse("h6"), BLACK_PAWN, NONE, NONE, static_cast<MoveType>(0)});
    game.make_move({parse("h1"), parse("h2"), WHITE_ROOK, NONE, NONE, static_cast<MoveType>(0)});
    game.make_move({parse("h8"), parse("h7"), BLACK_ROOK, NONE, NONE, static_cast<MoveType>(0)});
    std::cout << (game.get_current_board().castling_rights[BLACK] & KINGSIDE_CASTLE) << "\n";
    draw_board(game.get_current_board());
}
