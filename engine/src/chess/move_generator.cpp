#include <chess/move_generator.hpp>
#include <utils/bit_operations.hpp>
#include <chess/piece_attacks.hpp>
#include <iostream>


namespace mf::chess {
    namespace {
        uint64_t shift_pawn_single(const uint64_t board, const Colour clr) {
            return clr == WHITE ? board >> 8 : board << 8;
        }

        uint64_t shift_pawn_diagonal_left(const uint64_t board, const Colour clr) {
            return clr == WHITE ? board >> 9 : board << 7;
        }

        uint64_t shift_pawn_diagonal_right(const uint64_t board, const Colour clr) {
            return clr == WHITE ? board >> 7 : board << 9;
        }

        consteval uint64_t rank_board(const int n) {
            return static_cast<uint64_t>(0xFF) << (8 * (8 - n));
        }

        consteval uint64_t file_board(const char f)
        {
            return static_cast<uint64_t>(0x0101010101010101) << (f - 'a');
        }

        uint8_t closest_diagonal_ray_blocker(const uint64_t blockers, const DiagonalDirection d) {
            if (d == NORTHWEST || d == NORTHEAST) {
                return utils::msb_index(blockers);
            } else {
                return utils::lsb_index(blockers);
            }
        }

        uint8_t closest_cardinal_ray_blocker(const uint64_t blockers, const CardinalDirection d) {
            if (d == WEST || d == NORTH) {
                return utils::msb_index(blockers);
            } else {
                return utils::lsb_index(blockers);
            }
        }
    }


    int MoveGenerator::gen_pseudo_legal(const BoardState& board_state, const std::span<Move> moves) {
        int count = 0;
        if (board_state.side_to_move == WHITE) {
            generate_pawn_quiet_moves<WHITE>(board_state, moves, count);
            generate_pawn_captures<WHITE>(board_state, moves, count);
            generate_knight_moves<WHITE>(board_state, moves, count);
            generate_bishop_moves<WHITE>(board_state, moves, count);
            generate_rook_moves<WHITE>(board_state, moves, count);
            generate_queen_moves<WHITE>(board_state, moves, count);
            generate_king_moves<WHITE>(board_state, moves, count);
        } else {
            generate_pawn_quiet_moves<BLACK>(board_state, moves, count);
            generate_pawn_captures<BLACK>(board_state, moves, count);
            generate_knight_moves<BLACK>(board_state, moves, count);
            generate_bishop_moves<BLACK>(board_state, moves, count);
            generate_rook_moves<BLACK>(board_state, moves, count);
            generate_queen_moves<BLACK>(board_state, moves, count);
            generate_king_moves<BLACK>(board_state, moves, count);
        }
        return count;
    }

    template<Colour clr>
    bool MoveGenerator::is_attacked(const BoardState& board_state, const Square square) {
        return
            is_attacked_by_diagonal<clr>(board_state, square) ||
            is_attacked_by_cardinal<clr>(board_state, square) ||
            is_attacked_by_pawn<clr>(board_state, square) ||
            is_attacked_by_knight<clr>(board_state, square) ||
            is_attacked_by_king<clr>(board_state, square);
    }


    template<Colour clr>
    bool MoveGenerator::is_attacked_by_diagonal(const BoardState& board_state, const Square square) {
        const uint64_t  attacker_board = clr == WHITE ?
            board_state.bitboards[BLACK_BISHOP] | board_state.bitboards[BLACK_QUEEN] :
            board_state.bitboards[WHITE_BISHOP] | board_state.bitboards[WHITE_QUEEN];

        for (const auto d : DIAGONAL_DIRECTIONS) {
            const uint64_t ray = DIAGONAL_ATTACKS[d][square];
            if (const uint64_t blockers = ray & board_state.all_pieces; blockers & attacker_board) {
                if (const uint8_t block_square = closest_diagonal_ray_blocker(blockers, d); attacker_board & bit(block_square)) {
                    return true;
                }
            }
        }
        return false;
    }

    template<Colour clr>
    bool MoveGenerator::is_attacked_by_cardinal(const BoardState& board_state, const Square square) {
        const uint64_t attacker_board = clr == WHITE ?
            board_state.bitboards[BLACK_ROOK] | board_state.bitboards[BLACK_QUEEN] :
            board_state.bitboards[WHITE_ROOK] | board_state.bitboards[WHITE_QUEEN];

        for (const auto d : CARDINAL_DIRECTIONS) {
            const uint64_t ray = CARDINAL_ATTACKS[d][square];
            if (const uint64_t blockers = ray & board_state.all_pieces; blockers & attacker_board) {
                if (const uint8_t block_square = closest_cardinal_ray_blocker(blockers, d); attacker_board & bit(block_square)) {
                    return true;
                }
            }
        }
        return false;
    }

    template<Colour clr>
    bool MoveGenerator::is_attacked_by_pawn(const BoardState& board_state, const Square square) {
        const uint64_t attacker_board = clr == WHITE ? board_state.bitboards[BLACK_PAWN] : board_state.bitboards[WHITE_PAWN];
        constexpr Colour op_clr = switch_colour(clr);
        const uint64_t attack_board = shift_pawn_diagonal_left(attacker_board & ~file_board('a'), op_clr) | shift_pawn_diagonal_right(attacker_board & ~file_board('h'), op_clr);

        if (attack_board & bit(square)) {
            return true;
        }
        return false;
    }

    template<Colour clr>
    bool MoveGenerator::is_attacked_by_knight(const BoardState& board_state, const Square square) {
        const uint64_t attacker_board = clr == WHITE ? board_state.bitboards[BLACK_KNIGHT] : board_state.bitboards[WHITE_KNIGHT];

        if (KNIGHT_ATTACKS[square] & attacker_board) {
            return true;
        }
        return false;
    }

    template<Colour clr>
    bool MoveGenerator::is_attacked_by_king(const BoardState& board_state, const Square square) {
        const uint64_t attacker_board = clr == WHITE ? board_state.bitboards[BLACK_KING] : board_state.bitboards[WHITE_KING];

        if (KING_ATTACKS[square] & attacker_board) {
            return true;
        }
        return false;
    }


    template<Colour clr>
    void MoveGenerator::generate_moves_from_board(const PieceType type, const uint8_t piece_square, const uint64_t move_board, const BoardState& board_state, std::span<Move> moves, int& index) {
        const uint64_t op_board = board_state.occupancy[switch_colour(clr)];
        uint64_t capture_moves = move_board & op_board;
        uint64_t quiet_moves = move_board & ~op_board;

        while (capture_moves) {
            const uint8_t target_square = utils::lsb_index(capture_moves);
            utils::remove_lsb(capture_moves);
            const PieceType captured = board_state.piece_at(target_square);

            moves[index++] = {
                piece_square,
                target_square,
                type,
                captured,
                NONE,
                CAPTURE
            };
        }
        while (quiet_moves) {
            const uint8_t target_square = utils::lsb_index(quiet_moves);
            utils::remove_lsb(quiet_moves);

            moves[index++] = {
                piece_square,
                target_square,
                type,
                NONE,
                NONE,
                QUIET
            };
        }
    }

    template<Colour clr>
    void MoveGenerator::generate_knight_moves(const BoardState& board_state, const std::span<Move> moves, int& index) {
        constexpr PieceType type = clr == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
        uint64_t board = board_state.bitboards[type];

        while (board) {
            const uint8_t piece_square = utils::lsb_index(board);
            utils::remove_lsb(board);

            const uint64_t move_board = KNIGHT_ATTACKS[piece_square] & ~board_state.occupancy[clr];
            generate_moves_from_board<clr>(type, piece_square, move_board, board_state, moves, index);
        }
    }

    template<Colour clr>
    void MoveGenerator::generate_pawn_quiet_moves(const BoardState& board_state, std::span<Move> moves, int& index) {
        constexpr PieceType type = clr == WHITE ? WHITE_PAWN : BLACK_PAWN;
        const uint64_t board = board_state.bitboards[type];

        const uint64_t pawn_double_board = clr == WHITE ? rank_board(4) : rank_board(5);
        const uint64_t promotion_board = clr == WHITE ? rank_board(8) : rank_board(1);
        const auto pawn_direction = clr == WHITE ? WHITE_PAWN_DIRECTION : BLACK_PAWN_DIRECTION;
        const auto promotion_pieces = clr == WHITE ? WHITE_PROMOTION_PIECES : BLACK_PROMOTION_PIECES;

        const auto single_move_board = shift_pawn_single(board, clr) & ~board_state.all_pieces;
        auto non_promotion_move_board = single_move_board & ~promotion_board;
        auto promotion_move_board = single_move_board & promotion_board;
        auto double_move_board = shift_pawn_single(single_move_board, clr) & pawn_double_board & ~board_state.all_pieces;

        while (non_promotion_move_board) {
            const uint8_t target_square = utils::lsb_index(non_promotion_move_board);
            utils::remove_lsb(non_promotion_move_board);
            const uint8_t piece_square = target_square - pawn_direction;

            moves[index++] = {
                piece_square,
                target_square,
                type,
                NONE,
                NONE,
                QUIET
            };
        }

        while (promotion_move_board) {
            const uint8_t target_square = utils::lsb_index(promotion_move_board);
            utils::remove_lsb(promotion_move_board);
            const uint8_t piece_square = target_square - pawn_direction;

            for (const auto promotion_piece : promotion_pieces) {
                moves[index++] = {
                    piece_square,
                    target_square,
                    type,
                    NONE,
                    promotion_piece,
                    PROMOTION
                };
            }
        }

        while (double_move_board) {
            const uint8_t target_square = utils::lsb_index(double_move_board);
            utils::remove_lsb(double_move_board);
            const uint8_t piece_square = target_square - pawn_direction * 2;

            moves[index++] = {
                piece_square,
                target_square,
                type,
                NONE,
                NONE,
                PAWN_DOUBLE
            };
        }
    }

    template<Colour clr>
    void MoveGenerator::generate_pawn_captures(const BoardState& board_state, std::span<Move> moves, int& index) {
        constexpr Colour op_clr = switch_colour(clr);
        constexpr PieceType type = clr == WHITE ? WHITE_PAWN : BLACK_PAWN;
        const uint64_t board = board_state.bitboards[type];

        constexpr uint64_t promotion_board = clr == WHITE ? rank_board(8) : rank_board(1);
        constexpr auto pawn_direction = clr == WHITE ? WHITE_PAWN_DIRECTION : BLACK_PAWN_DIRECTION;
        constexpr auto promotion_pieces = clr == WHITE ? WHITE_PROMOTION_PIECES : BLACK_PROMOTION_PIECES;

        // Check for en-passent
        if (board_state.en_passent_target) {
            const uint8_t en_passent_square = utils::bit_index(board_state.en_passent_target);
            const PieceType captured = clr == WHITE ? BLACK_PAWN : WHITE_PAWN;

            // Check diagonal left and diagonal right of en_passent square for a pawn
            if (const uint64_t diagonal_left_piece_board = shift_pawn_diagonal_left(board_state.en_passent_target, op_clr); diagonal_left_piece_board & board) {
                const uint8_t piece_square = utils::bit_index(diagonal_left_piece_board);
                moves[index++] = {
                    piece_square,
                    en_passent_square,
                    type,
                    captured,
                    NONE,
                    CAPTURE | EN_PASSENT
                };
            }
            if (const uint64_t diagonal_right_piece_board = shift_pawn_diagonal_right(board_state.en_passent_target, op_clr); diagonal_right_piece_board & board) {
                const uint8_t piece_square = utils::bit_index(diagonal_right_piece_board);
                moves[index++] = {
                    piece_square,
                    en_passent_square,
                    type,
                    captured,
                    NONE,
                    CAPTURE | EN_PASSENT
                };
            }
        }

        auto left_attack_move_board = shift_pawn_diagonal_left(board & ~file_board('a'), clr) & board_state.occupancy[op_clr];
        while (left_attack_move_board) {
            const uint8_t target_square = utils::lsb_index(left_attack_move_board);
            utils::remove_lsb(left_attack_move_board);

            const uint8_t piece_square = target_square - pawn_direction + 1;
            const PieceType captured = board_state.piece_at(target_square);

            if (const uint64_t piece_map = bit(target_square); promotion_board & piece_map) {
                for (const auto promotion_piece : promotion_pieces) {
                    moves[index++] = {
                        piece_square,
                        target_square,
                        type,
                        captured,
                        promotion_piece,
                        CAPTURE | PROMOTION
                    };
                }
            } else {
                moves[index++] = {
                    piece_square,
                    target_square,
                    type,
                    captured,
                    NONE,
                    CAPTURE
                };
            }
        }

        auto right_attack_move_board = shift_pawn_diagonal_right(board & ~file_board('h'), clr) & board_state.occupancy[op_clr];
        while (right_attack_move_board) {
            const uint8_t target_square = utils::lsb_index(right_attack_move_board);
            utils::remove_lsb(right_attack_move_board);

            const uint8_t piece_square = target_square - pawn_direction - 1;
            const PieceType captured = board_state.piece_at(target_square);

            if (const uint64_t piece_map = bit(target_square); promotion_board & piece_map) {
                for (const auto promotion_piece : promotion_pieces) {
                    moves[index++] = {
                        piece_square,
                        target_square,
                        type,
                        captured,
                        promotion_piece,
                        CAPTURE | PROMOTION
                    };
                }
            } else {
                moves[index++] = {
                    piece_square,
                    target_square,
                    type,
                    captured,
                    NONE,
                    CAPTURE
                };
            }
        }
    }

    template<Colour clr>
    void MoveGenerator::generate_diagonal_moves(const PieceType type, const BoardState& board_state, const std::span<Move> moves, int& index) {
        auto board = board_state.bitboards[type];

        while (board) {
            const uint8_t piece_square = utils::lsb_index(board);
            utils::remove_lsb(board);

            uint64_t move_board = 0;
            for (const auto d : DIAGONAL_DIRECTIONS) {
                const uint64_t ray = DIAGONAL_ATTACKS[d][piece_square];
                if (const uint64_t blockers = board_state.all_pieces & ray) {
                    move_board |= ray & ~DIAGONAL_ATTACKS[d][closest_diagonal_ray_blocker(blockers, d)];
                } else {
                    move_board |= ray;
                }
            }
            move_board &= ~board_state.occupancy[clr];

            generate_moves_from_board<clr>(type, piece_square, move_board, board_state, moves, index);
        }
    }

    template<Colour clr>
    void MoveGenerator::generate_cardinal_moves(const PieceType type, const BoardState& board_state, const std::span<Move> moves, int& index) {
        auto board = board_state.bitboards[type];

        while (board) {
            const uint8_t piece_square = utils::lsb_index(board);
            utils::remove_lsb(board);

            uint64_t move_board = 0;
            for (const auto d : CARDINAL_DIRECTIONS) {
                const uint64_t ray = CARDINAL_ATTACKS[d][piece_square];
                if (const uint64_t blockers = board_state.all_pieces & ray) {
                    move_board |= ray & ~CARDINAL_ATTACKS[d][closest_cardinal_ray_blocker(blockers, d)];
                } else {
                    move_board |= ray;
                }
            }
            move_board &= ~board_state.occupancy[clr];

            generate_moves_from_board<clr>(type, piece_square, move_board, board_state, moves, index);
        }
    }

    template<Colour clr>
    void MoveGenerator::generate_bishop_moves(const BoardState& board_state, const std::span<Move> moves, int& index) {
        constexpr PieceType type = clr == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
        generate_diagonal_moves<clr>(type, board_state, moves, index);
    }

    template<Colour clr>
    void MoveGenerator::generate_rook_moves(const BoardState& board_state, const std::span<Move> moves, int& index) {
        constexpr PieceType type = clr == WHITE ? WHITE_ROOK : BLACK_ROOK;
        generate_cardinal_moves<clr>(type, board_state, moves, index);
    }

    template<Colour clr>
    void MoveGenerator::generate_queen_moves(const BoardState& board_state, const std::span<Move> moves, int& index) {
        constexpr PieceType type = clr == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
        generate_diagonal_moves<clr>(type, board_state, moves, index);
        generate_cardinal_moves<clr>(type, board_state, moves, index);
    }

    template<Colour clr>
    void MoveGenerator::generate_king_moves(const BoardState& board_state, const std::span<Move> moves, int& index) {
        constexpr PieceType type = clr == WHITE ? WHITE_KING : BLACK_KING;
        uint64_t board = board_state.bitboards[type];

        while (board) {
            const uint8_t piece_square = utils::lsb_index(board);
            utils::remove_lsb(board);

            const uint64_t move_board = KING_ATTACKS[piece_square] & ~board_state.occupancy[clr];
            generate_moves_from_board<clr>(type, piece_square, move_board, board_state, moves, index);
        }

        if constexpr (clr == WHITE) {
            if (board_state.castling_rights[clr] ^ KINGSIDE_CASTLE &&
                !is_attacked<clr>(board_state, sq('e', 1)) &&
                !is_attacked<clr>(board_state, sq('f', 1)) &&
                board_state.piece_at(sq('f', 1)) == NONE &&
                board_state.piece_at(sq('g', 1)) == NONE) {
                    moves[index++] = {
                        sq('e', 1),
                        sq('g', 1),
                        type,
                        NONE,
                        NONE,
                        CASTLE
                };
            } else if (board_state.castling_rights[clr] ^ QUEENSIDE_CASTLE &&
                !is_attacked<clr>(board_state, sq('e', 1))&&
                !is_attacked<clr>(board_state, sq('d', 1)) &&
                board_state.piece_at(sq('d', 1)) == NONE &&
                board_state.piece_at(sq('c', 1)) == NONE) {
                    moves[index++] = {
                        sq('e', 1),
                        sq('c', 1),
                        type,
                        NONE,
                        NONE,
                        CASTLE
                };
            }
        } else {
            if (board_state.castling_rights[clr] ^ KINGSIDE_CASTLE &&
                !is_attacked<clr>(board_state, sq('e', 8)) &&
                !is_attacked<clr>(board_state, sq('f', 8)) &&
                board_state.piece_at(sq('f', 8)) == NONE &&
                board_state.piece_at(sq('g', 8)) == NONE) {
                    moves[index++] = {
                        sq('e', 8),
                        sq('g', 8),
                        type,
                        NONE,
                        NONE,
                        CASTLE
                };
            } else if (board_state.castling_rights[clr] ^ QUEENSIDE_CASTLE &&
                !is_attacked<clr>(board_state, sq('e', 8)) &&
                !is_attacked<clr>(board_state, sq('d', 8)) &&
                board_state.piece_at(sq('d', 8)) == NONE &&
                board_state.piece_at(sq('c', 8)) == NONE) {
                    moves[index++] = {
                        sq('e', 8),
                        sq('c', 8),
                        type,
                        NONE,
                        NONE,
                        CASTLE
                    };
            }
        }
    }
}
