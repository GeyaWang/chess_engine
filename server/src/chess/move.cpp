#include <chess/move.hpp>
#include <utils/parser.hpp>
#include <stdexcept>


chess::Move chess::Move::parse(const std::string &s) {
    utils::Parser parser{s};

    PieceType piece;
    if (parser.consume('K')) {
        piece = KING;
    } else if (parser.consume('Q')) {
        piece = QUEEN;
    } else if (parser.consume('R')) {
        piece = ROOK;
    } else if (parser.consume('B')) {
        piece = BISHOP;
    } else if (parser.consume('N')) {
        piece = KNIGHT;
    } else if (parser.consume('P')) {
        piece = PAWN;
    } else {
        throw std::invalid_argument("invalid piece type");
    }

    parser.expect(' ');

    return Move(piece, {'A', 1}, {'A', 1});
}
