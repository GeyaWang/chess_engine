#include <chess/game.hpp>
#include <chess/move_generator.hpp>
#include <iostream>
#include <bitset>
#include <array>


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
    game.make_move("e2", "e3", NONE);
    game.make_move("a7", "a6", NONE);
    game.make_move("f1", "e2", NONE);
    game.make_move("a6", "a5", NONE);
    game.make_move("g1", "f3", NONE);
    game.make_move("a5", "a4", NONE);

    std::array<Move, 256> moves{};
    int count = MoveGenerator::gen_pseudo_legal(game.get_current_board(), moves);

    for (int i = 0; i < count; i++) {
        game.make_move(moves[i]);
        draw_board(game.get_current_board());
        game.undo_move();
    }
}
