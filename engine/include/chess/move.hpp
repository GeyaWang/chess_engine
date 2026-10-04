#pragma once

#include <chess/board_state.hpp>
#include <iostream>


namespace mf::chess {
    enum MoveType : uint8_t {
        QUIET = 0,
        CAPTURE = 1 << 0,
        PAWN_DOUBLE = 1 << 1,
        EN_PASSENT = 1 << 2,
        CASTLE = 1 << 3,
        PROMOTION = 1 << 4
    };


    inline std::string square_to_alg(const Square square) {
        if (square < 0 || square >= 64) return "--";
        return std::string{static_cast<char>('a' + square % 8), static_cast<char>('8' - square / 8)};
    }

    static constexpr char PIECE_CHARS[] = "PNBRQKpnbrqk.";


    struct Move {
        Square from = 64;
        Square to = 64;
        PieceType piece = NONE;
        PieceType captured = NONE;
        PieceType promotion = NONE;
        uint8_t move_type = 0;

        bool operator==(const Move& rhs) const { return from == rhs.from && to == rhs.to && promotion == rhs.promotion; }

        friend std::ostream& operator<<(std::ostream& os, const Move& m) {
            return os << "{" << square_to_alg(m.from) << "->" << square_to_alg(m.to) << ",piece=" << PIECE_CHARS[m.piece] << ",captured=" << PIECE_CHARS[m.captured] << ",promotion=" << PIECE_CHARS[m.promotion] << ",type=" << static_cast<int>(m.move_type) << "}";
        }
    };
}
