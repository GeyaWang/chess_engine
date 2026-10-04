#include <engine/search.hpp>
#include <chess/board_state.hpp>
#include <chess/move_generator.hpp>
#include <vector>
#include <random>
#include <array>


namespace mf::engine {
    constexpr std::array PIECE_VAL = {100, 300, 300, 500, 900, 0, -100, -300, -300, -500, -900, 0};

    int Search::evaluate(const chess::BoardState& board_state) {
        const auto bitboards = board_state.bitboards;
        int score = 0;
        for (const auto piece : chess::ALL_PIECES) {
            score += std::popcount(bitboards[piece]) * PIECE_VAL[piece];
        }
        return score;
    }

    int Search::minimax(chess::Game& game, const int depth, uint64_t& nodes_searched) {
        const chess::BoardState& board_state = game.get_current_board();

        nodes_searched++;

        const auto terminal_state = game.get_terminal_state();
        if (terminal_state == chess::WHITE_WIN) return 100000 + depth;
        if (terminal_state == chess::BLACK_WIN) return -(100000 + depth);
        if (terminal_state == chess::DRAW)      return 0;

        if (depth <= 0) return evaluate(board_state);

        std::array<chess::Move, 218> moves;
        const int move_count = chess::MoveGenerator::gen_pseudo_legal(board_state, moves);

        if (board_state.side_to_move == chess::WHITE) {
            int best_score = -1000000;
            for (int i = 0; i < move_count; i++) {
                game.make_move(moves[i]);
                if (!game.get_current_board().is_king_attacked(chess::WHITE)) {
                    const int score = minimax(game, depth - 1, nodes_searched);
                    best_score = std::max(score, best_score);
                }
                game.undo_move();
            }
            return best_score;
        }

        // side_to_move == BLACK
        int best_score = 1000000;
        for (int i = 0; i < move_count; i++) {
            game.make_move(moves[i]);
            if (!game.get_current_board().is_king_attacked(chess::BLACK)) {
                const int score = minimax(game, depth - 1, nodes_searched);
                best_score = std::min(score, best_score);
            }
            game.undo_move();
        }
        return best_score;
    }

    SearchMove Search::best_move(chess::Game& game, const int depth) {
        const chess::BoardState& board_state = game.get_current_board();

        if (game.get_terminal_state() != chess::NON_TERMINAL) {
            // Return fallback
            return {
                0,
                {0, 0, chess::NONE, chess::NONE, chess::NONE, 0}
            };
        }

        uint64_t nodes_searched = 0;
        std::array<chess::Move, 218> moves;
        const int move_count = chess::MoveGenerator::gen_pseudo_legal(board_state, moves);
        std::vector<chess::Move> best_moves{};

        if (board_state.side_to_move == chess::WHITE) {
            int best_score = -1000000;
            for (int i = 0; i < move_count; i++) {
                game.make_move(moves[i]);
                if (!game.get_current_board().is_king_attacked(chess::WHITE)) {
                    if (const int score = minimax(game, depth - 1, nodes_searched); score > best_score) {
                        best_score = score;
                        best_moves = {moves[i]};
                    } else if (score == best_score) {
                        best_moves.emplace_back(moves[i]);
                    }
                }
                game.undo_move();
            }
        } else {
            // side_to_move == BLACK
            int best_score = 1000000;
            for (int i = 0; i < move_count; i++) {
                game.make_move(moves[i]);
                if (!game.get_current_board().is_king_attacked(chess::BLACK)) {
                    if (const int score = minimax(game, depth - 1, nodes_searched); score < best_score) {
                        best_score = score;
                        best_moves = {moves[i]};
                    } else if (score == best_score) {
                        best_moves.emplace_back(moves[i]);
                    }
                }
                game.undo_move();
            }
        }

        if (best_moves.empty()) {
            return {nodes_searched, {}};
        }

        std::uniform_int_distribution<std::size_t> dist(0, best_moves.size() - 1);
        const chess::Move chosen_move = best_moves[dist(gen_)];
        return {nodes_searched, chosen_move};
    }
}
