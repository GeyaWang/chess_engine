#include <chess/game.hpp>
#include <iostream>
#include <bitset>
#include <chess/move_generator.hpp>
#include <engine/search.hpp>
#include <sstream>

#include "../include/chess/move.hpp"


namespace mf::chess {
    struct Move;
}

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


inline std::string square_to_alg(const mf::chess::Square square) {
    if (square < 0 || square >= 64) return "--";
    return std::string{static_cast<char>('a' + square % 8), static_cast<char>('8' - square / 8)};
}

static constexpr char PIECE_CHARS[] = "PNBRQKpnbrqk.";


std::string move_str(const mf::chess::Move& m) {
    std::stringstream ss;
    ss << "{" + square_to_alg(m.from) << "->" << square_to_alg(m.to) << ",piece=" << PIECE_CHARS[m.piece] << ",captured=" << PIECE_CHARS[m.captured] << ",promotion=" << PIECE_CHARS[m.promotion] << ",type=" << static_cast<int>(m.move_type) << "}";
    return ss.str();
}


int main() {
    using namespace mf::chess;

    Game game{};
    game.set_fen("rnbqkbnr/pppppp1p/P7/8/8/1P6/2PPPPPp/RNBQKBNR b KQkq - 0 5");
    draw_board(game.get_current_board());
    std::cout << "turn=" << (game.get_current_board().side_to_move == WHITE) << "\n";

    std::array<Move, 218> moves;
    const int count = MoveGenerator::gen_pseudo_legal(game.get_current_board(), moves);

    for (int i = 0; i < count; i++) {
        std::cout << move_str(moves[i]) << "\n";
    }

    mf::engine::Search search{};
    const auto [n, move] = search.best_move(game, 3);
    std::cout << n << " " << move_str(move) << "\n";

    std::cout << "king_attacked = " << game.get_current_board().is_king_attacked(BLACK) << "\n";
}
