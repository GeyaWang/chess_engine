#pragma once

#include <array>
#include <cstdint>


namespace mf::chess {
    enum Turn : bool {
        WHITE,
        BLACK
    };

    enum PieceType : uint8_t {
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
        EMPTY,
    };

    constexpr std::array ALL_PIECES = {WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING, BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING};
    constexpr std::array WHITE_PIECES = {WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING};
    constexpr std::array BLACK_PIECES = {BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING};
    constexpr std::array WHITE_CAPTURABLE_PIECES = {WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN};
    constexpr std::array BLACK_CAPTURABLE_PIECES = {BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN};


    template <char col, int row>
    constexpr uint8_t square() {
        static_assert(col >= 'a' && col <= 'h', "column must be 'a' to 'h'");
        static_assert(row >= 1 && row <= 8, "row must be 1 to 8");
        return (8 - row) * 8 + (col - 'a');
    }


    struct BoardState {
        std::array<uint64_t, ALL_PIECES.size()> bitboards{};
        Turn turn = WHITE;

        BoardState() = default;

        static BoardState create_default();
        static void draw(const BoardState& board_state);

        [[nodiscard]] PieceType piece_at(uint8_t pos) const;
        void set_at(PieceType piece, uint8_t pos);
    };
}
