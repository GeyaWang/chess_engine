#include "chess/game.hpp"


namespace mf::chess {
    Game::Game() {
        board_states_[0] = BoardState::create_default();
    }

    BoardState &Game::get_current_board() {
        return board_states_[current_index_];
    }

    void Game::move(const Move &move) {
        const auto new_index = current_index_ + 1;
        board_states_[new_index] = board_states_[current_index_];

        const uint64_t from_board = static_cast<uint64_t>(1) << move.from;
        const uint64_t to_board = static_cast<uint64_t>(1) << move.to;
        const uint64_t move_board = from_board | to_board;

        const auto ally_pieces = board_states_[current_index_].turn == WHITE ? WHITE_PIECES : BLACK_PIECES;
        const auto enemy_capturable_pieces = board_states_[current_index_].turn == WHITE ? BLACK_CAPTURABLE_PIECES : WHITE_CAPTURABLE_PIECES;

        // Move piece at "from" position
        PieceType move_piece = EMPTY;
        for (const auto i : ally_pieces) {
            if (board_states_[current_index_].bitboards[i] & from_board) {
                board_states_[new_index].bitboards[i] ^= move_board;
                move_piece = i;
                break;
            }
        }

        // Capture piece at "to" position
        for (const auto i : enemy_capturable_pieces) {
            if (board_states_[current_index_].bitboards[i] & to_board) {
                board_states_[new_index].bitboards[i] ^= to_board;
                break;
            }
        }

        current_index_++;
    }

    void Game::undo() {
        if (current_index_ > 0) {
            current_index_--;
        }
    }

    void Game::gen_piece_moves(MoveList &move_list, const PieceType piece_type, const uint8_t pos) {
        switch (piece_type) {
            case WHITE_PAWN:
            case BLACK_PAWN: {
                break;
            }

            case WHITE_KNIGHT:
            case BLACK_KNIGHT: {
                break;
            }

            case WHITE_BISHOP:
            case BLACK_BISHOP: {
                // Top-left
                uint8_t p = pos;
                for (int i = 0; i < pos % 8; i++) {
                    p -= 9;
                    if (p < 0) break;
                    move_list.add({pos, p});
                }

                // Top-right
                p = pos;
                for (int i = 0; i < 7 - pos % 8; i++) {
                    p -= 7;
                    if (p < 0) break;
                    move_list.add({pos, p});
                }

                // Bottom-left
                p = pos;
                for (int i = 0; i < pos % 8; i++) {
                    p += 7;
                    if (p >= 64) break;
                    move_list.add({pos, p});
                }

                // Bottom-right
                p = pos;
                for (int i = 0; i < 7 - pos % 8; i++) {
                    p += 9;
                    if (p >= 64) break;
                    move_list.add({pos, p});
                }
                break;
            }

            case WHITE_ROOK:
            case BLACK_ROOK: {
                break;
            }

            case WHITE_QUEEN:
            case BLACK_QUEEN: {
                break;
            }

            case WHITE_KING:
            case BLACK_KING: {
                break;
            }

            default:
                break;
        }
    }

    MoveList Game::gen_moves() {
        MoveList move_list{};

        // Iterate through ally pieces
        for (const auto piece_type : get_current_board().turn == WHITE ? WHITE_PIECES : BLACK_PIECES) {
            for (uint8_t pos = 0; pos < 64; pos++) {
                if (const uint64_t mask = static_cast<uint64_t>(1) << pos; mask & get_current_board().bitboards[piece_type]) {
                    gen_piece_moves(move_list, piece_type, pos);
                }
            }
        }

        return move_list;
    }

    bool Game::is_legal(const Move &move) {
        const auto [moves, count] = gen_moves();
        for (int i = 0; i < count; i++) {
            if (moves[i] == move) {
                return true;
            }
        }
        return false;
    }
}
