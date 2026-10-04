#include <engine/search.hpp>
#include <chess/board_state.hpp>
#include <chess/move_generator.hpp>
#include <vector>
#include <random>
#include <array>


namespace mf::engine {
    constexpr std::array PIECE_VAL = {100, 300, 300, 500, 900, 0, -100, -300, -300, -500, -900, 0};

    int Search::evaluate(const chess::BoardState& board_state) {
        const auto& bitboards = board_state.bitboards;
        int score = 0;
        for (const auto piece : chess::ALL_PIECES) {
            score += std::popcount(bitboards[piece]) * PIECE_VAL[piece];
        }
        return score;
    }

    int Search::minimax(uint64_t& nodes_searched, chess::Game& game, const int depth, int alpha, int beta) {
        const chess::Colour ally_clr = game.get_current_board().side_to_move;

        nodes_searched++;

        if (depth <= 0) {
            switch (game.get_terminal_state()) {
                case chess::WHITE_WIN:
                case chess::BLACK_WIN:
                    return -100000 - depth;
                case chess::DRAW:
                    return 0;
                default:
                    return ally_clr == chess::WHITE ? evaluate(game.get_current_board()) : -evaluate(game.get_current_board());
            }
        }

        if (game.is_soft_draw()) return 0;

        std::array<chess::Move, 218> moves;
        const int move_count = chess::MoveGenerator::gen_pseudo_legal(game.get_current_board(), moves);
        bool is_terminal = true;

        for (int i = 0; i < move_count; i++) {
            game.make_move(moves[i]);
            if (ally_clr == chess::WHITE ? !game.get_current_board().is_king_attacked(chess::WHITE) : !game.get_current_board().is_king_attacked(chess::BLACK)) {
                is_terminal = false;
                const int score = -minimax(nodes_searched, game, depth - 1, -beta, -alpha);
                if (score > alpha) alpha = score;
            }
            game.undo_move();

            if (alpha >= beta) break;
        }

        if (is_terminal) {
            if (ally_clr == chess::WHITE ? game.get_current_board().is_king_attacked(chess::WHITE) : game.get_current_board().is_king_attacked(chess::BLACK)) {
                return -100000 - depth;
            }
            return 0;
        }

        return alpha;
    }

    SearchMove Search::best_move(chess::Game& game, const int depth) {
        const chess::Colour ally_clr = game.get_current_board().side_to_move;

        if (game.get_terminal_state() != chess::NON_TERMINAL)  return { 0, {} };

        uint64_t nodes_searched = 0;
        std::array<chess::Move, 218> moves;
        const int move_count = chess::MoveGenerator::gen_pseudo_legal(game.get_current_board(), moves);
        chess::Move best_move = moves[0];

        int alpha = -1000000;
        for (int i = 0; i < move_count; i++) {
            game.make_move(moves[i]);
            if (ally_clr == chess::WHITE ? !game.get_current_board().is_king_attacked(chess::WHITE) : !game.get_current_board().is_king_attacked(chess::BLACK)) {
                const int score = -minimax(nodes_searched, game, depth - 1, -1000000, -alpha);
                if (score > alpha) {
                    alpha = score;
                    best_move = moves[i];
                }
            }
            game.undo_move();
        }

        return {nodes_searched, best_move};
    }
}
