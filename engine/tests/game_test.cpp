#include <chess/game.hpp>
#include <iostream>
#include <bitset>
#include <chess/move_generator.hpp>
#include <engine/search.hpp>


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
    game.set_fen("7k/8/5K2/8/8/8/8/6R1 w - - 0 1");
    draw_board(game.get_current_board());
    std::cout << "turn=" << (game.get_current_board().side_to_move == WHITE) << "\n";
    // game.make_move({parse("f1"), parse("e2"), WHITE_BISHOP, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("g1"), parse("f3"), WHITE_KNIGHT, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("e1"), parse("g1"), WHITE_KING, NONE, NONE, CASTLE});

    // game.make_move({parse("h2"), parse("h3"), WHITE_PAWN, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("h7"), parse("h6"), BLACK_PAWN, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("h1"), parse("h2"), WHITE_ROOK, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("h8"), parse("h7"), BLACK_ROOK, NONE, NONE, static_cast<MoveType>(0)});
    // std::cout << (game.get_current_board().castling_rights[BLACK] & KINGSIDE_CASTLE) << "\n";
    // draw_board(game.get_current_board());

    game.make_move({parse("f6"), parse("f7"), WHITE_KING, NONE, NONE, static_cast<MoveType>(0)});
    // game.make_move({parse("h8"), parse("h7"), BLACK_KING, NONE, NONE, static_cast<MoveType>(0)});

    draw_board(game.get_current_board());

    mf::engine::Search search{};
    const auto [n, move] = search.best_move(game, 3);
    std::cout << n << " " << move << "\n";

    // std::array<Move, 256> moves{};
    // int count = MoveGenerator::gen_pseudo_legal(game.get_current_board(), moves);
    //
    // for (int i = 0; i < count; i++) {
    //     std::cout << moves[i] << "\n";
    //     game.make_move(moves[i]);
    //     // draw_board(game.get_current_board());
    //     game.undo_move();
    // }

    std::cout << "king_attacked = " << game.get_current_board().is_king_attacked(BLACK) << "\n";
}
