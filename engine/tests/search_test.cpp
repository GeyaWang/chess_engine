#include <chess/game.hpp>
#include <chess/move_generator.hpp>
#include <engine/search.hpp>
#include <iostream>
#include <bitset>
#include <array>


using namespace mf::chess;


constexpr uint8_t square(const char col, const int row) {
    return (8 - row) * 8 + (col - 'a');
}


std::string square_to_str(const uint8_t square) {
    const uint8_t x = square % 8;
    const uint8_t y = square / 8;

    return std::string{
        static_cast<char>('a' + x), static_cast<char>('8' - y)
    };
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


void print_pseudo_legal(Game& game) {
    std::array<Move, 256> moves{};
    const int count = MoveGenerator::gen_pseudo_legal(game.get_current_board(), moves);

    for (int i = 0; i < count; i++) {
        game.make_move(moves[i]);
        draw_board(game.get_current_board());
        std::cout << "move " << square_to_str(moves[i].from) << " " << square_to_str(moves[i].to) << "\n";
        game.undo_move();
    }
}


bool is_terminal(Game& game) {
    const BoardState& board_state = game.get_current_board();
    const Colour clr = board_state.turn;

    std::array<Move, 218> moves{};
    const int move_count = MoveGenerator::gen_pseudo_legal(board_state, moves);

    for (int i = 0; i < move_count; i++) {
        game.make_move(moves[i]);
        draw_board(game.get_current_board());
        const bool is_illegal = clr == WHITE ? game.get_current_board().is_king_attacked(WHITE) : game.get_current_board().is_king_attacked(BLACK);
        if (is_illegal) {
            std::cout << "illegal\n";
        } else {
            std::cout << "legal\n";
        }
        game.undo_move();

        if (!is_illegal) {
            return false;
        }
    }
    return true;
}


int main() {
    Game game{};
    game.make_move(parse("e2"), parse("e3"), NONE);
    game.make_move(parse("a7"), parse("a6"), NONE);
    game.make_move(parse("d1"), parse("h5"), NONE);
    game.make_move(parse("a6"), parse("a5"), NONE);
    game.make_move(parse("h5"), parse("f7"), NONE);
    draw_board(game.get_current_board());

    const std::string side_to_move = game.get_current_board().turn == WHITE ? "WHITE" : "BLACK";
    std::cout << "colour to move=" << side_to_move << "\n";

    const bool is_term = is_terminal(game);
    std::cout << "is_terminal=" << is_term << "\n";

    // print_pseudo_legal(game);
    //
    // const auto [nodes_searched, best_move] = mf::engine::Search::best_move(game, 1);
    // std::cout << "move " << square_to_str(best_move.from) << " " << square_to_str(best_move.to) << "\n";
}
