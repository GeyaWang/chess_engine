#pragma once

#include <chess/board_state.hpp>


namespace mf::chess {
    enum MoveType : uint8_t {
        QUIET = 0,
        CAPTURE = 1 << 0,
        PAWN_DOUBLE = 1 << 1,
        EN_PASSENT = 1 << 2,
        CASTLE = 1 << 3,
        PROMOTION = 1 << 4
    };


    struct Move {
        Square from = 64;
        Square to = 64;
        PieceType piece = NONE;
        PieceType captured = NONE;
        PieceType promotion = NONE;
        uint8_t move_type = 0;

        bool operator==(const Move& rhs) const { return from == rhs.from && to == rhs.to; }
    };
}
