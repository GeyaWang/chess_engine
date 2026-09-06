#pragma once

#include <span>
#include <chess/move.hpp>
#include <chess/board_state.hpp>


namespace mf::chess {
    class MoveGenerator {
    public:
        static int gen_pseudo_legal(const BoardState& board_state, std::span<Move> moves);

        static bool is_attacked(const BoardState& board_state, Square square, Colour colour);

    private:
        static bool is_attacked_by_diagonal(const BoardState& board_state, Square square, Colour colour);
        static bool is_attacked_by_cardinal(const BoardState& board_state, Square square, Colour colour);
        static bool is_attacked_by_knight(const BoardState& board_state, Square square, Colour colour);
        static bool is_attacked_by_pawn(const BoardState& board_state, Square square, Colour colour);
        static bool is_attacked_by_king(const BoardState& board_state, Square square, Colour colour);

        static void generate_moves(Type type, Colour clr, uint8_t piece_square, uint64_t move_board, const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_pawn_quiet_moves(const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_pawn_captures(const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_knight_moves(const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_diagonal_moves(Type type, const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_cardinal_moves(Type type, const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_bishop_moves(const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_rook_moves(const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_queen_moves(const BoardState& board_state, std::span<Move> moves, int& index);
        static void generate_king_moves(const BoardState& board_state, std::span<Move> moves, int& index);
    };
}
