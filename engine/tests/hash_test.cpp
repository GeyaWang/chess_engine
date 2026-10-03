#include <chess/game.hpp>
#include <iostream>
#include <chess/zobrist_hash.hpp>


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
    game.make_move({parse("e2"), parse("e3"), WHITE_PAWN, NONE, NONE, static_cast<MoveType>(0)});
    game.make_move({parse("f1"), parse("e2"), WHITE_BISHOP, NONE, NONE, static_cast<MoveType>(0)});
    game.make_move({parse("g1"), parse("f3"), WHITE_KNIGHT, NONE, NONE, static_cast<MoveType>(0)});
    game.make_move({parse("e1"), parse("g1"), WHITE_KING, NONE, NONE, CASTLE});
    draw_board(game.get_current_board());

    const HashGenerator hash_gen{};
    auto hash = hash_gen.hash(game.get_current_board());
    std::cout << "Hash: " << hash << "\n";
}
