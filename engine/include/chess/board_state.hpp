#pragma once

#include <array>
#include <cstdint>


namespace mf::chess {
    consteval uint8_t sq(const char col, const int row) {
        return (8 - row) * 8 + (col - 'a');
    }

    constexpr uint64_t bit(const uint8_t square) {
        return static_cast<uint64_t>(1) << square;
    }


    enum Colour : uint8_t {
        WHITE = 0,
        BLACK = 1
    };

    enum Type : uint8_t {
        WHITE_PAWN,
        WHITE_KNIGHT,
        WHITE_BISHOP,
        WHITE_ROOK,
        WHITE_QUEEN,
        WHITE_KING,
        BLACK_PAWN,
        BLACK_KNIGHT,
        BLACK_BISHOP,
        BLACK_ROOK,
        BLACK_QUEEN,
        BLACK_KING,
        NONE,
    };

    constexpr Colour switch_colour(const Colour clr) {
        return clr == BLACK ? WHITE : BLACK;
    }

    constexpr Colour get_piece_colour(const Type piece) {
        return piece < BLACK_PAWN ? WHITE : BLACK;
    }

    enum CastlingRights : std::uint8_t
    {
        NONE_CASTLE      = 0,
        KINGSIDE_CASTLE  = 1 << 0,
        QUEENSIDE_CASTLE = 1 << 1,
    };

    constexpr std::array ALL_PIECES = {WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING, BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING};
    constexpr std::array WHITE_PIECES = {WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING};
    constexpr std::array BLACK_PIECES = {BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING};
    constexpr std::array WHITE_CAPTURABLE_PIECES = {WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN};
    constexpr std::array BLACK_CAPTURABLE_PIECES = {BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN};
    constexpr std::array WHITE_PROMOTION_PIECES = {WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN};
    constexpr std::array BLACK_PROMOTION_PIECES = {BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN};

    constexpr int WHITE_PAWN_DIRECTION = -8;
    constexpr int BLACK_PAWN_DIRECTION = 8;

    using Square = uint8_t;
    using Board = uint64_t;


    struct BoardState {
        std::array<Board, ALL_PIECES.size()> bitboards{};
        Board en_passent_target{};
        Colour side_to_move = WHITE;

        std::array<Board, 2> occupancy{};
        Board all_pieces{};
        std::array<uint8_t, 2> castling_rights{};

        static BoardState create_default();

        [[nodiscard]] Type piece_at(Square pos) const;
        void set_at(Type piece, Square pos);
        void apply_mask(Type piece, Board mask);
    };


    void draw_board(const BoardState& board_state);
}
