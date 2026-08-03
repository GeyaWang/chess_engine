#pragma once


namespace chess {
    enum PieceType {
        EMPTY,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
    };

    enum Colour {
        NONE,
        WHITE,
        BLACK
    };


    struct Piece {
        PieceType piece_type = EMPTY;
        Colour colour = NONE;

        Piece() = default;
        Piece(const PieceType pt, const Colour clr) : piece_type(pt), colour(clr) {}
    };
}
