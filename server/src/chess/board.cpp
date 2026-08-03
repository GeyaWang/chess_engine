#include <chess/board.hpp>


chess::Board chess::Board::create_empty() {
    constexpr Board board{};
    return board;
}

chess::Board chess::Board::create_default() {
    Board board;
    board.set_at({'A', 2}, {PAWN, WHITE});
    board.set_at({'B', 2}, {PAWN, WHITE});
    board.set_at({'C', 2}, {PAWN, WHITE});
    board.set_at({'D', 2}, {PAWN, WHITE});
    board.set_at({'E', 2}, {PAWN, WHITE});
    board.set_at({'F', 2}, {PAWN, WHITE});
    board.set_at({'G', 2}, {PAWN, WHITE});
    board.set_at({'H', 2}, {PAWN, WHITE});
    board.set_at({'A', 1}, {ROOK, WHITE});
    board.set_at({'B', 1}, {KNIGHT, WHITE});
    board.set_at({'C', 1}, {BISHOP, WHITE});
    board.set_at({'D', 1}, {QUEEN, WHITE});
    board.set_at({'E', 1}, {KING, WHITE});
    board.set_at({'F', 1}, {BISHOP, WHITE});
    board.set_at({'G', 1}, {KNIGHT, WHITE});
    board.set_at({'H', 1}, {ROOK, WHITE});
    board.set_at({'A', 7}, {PAWN, BLACK});
    board.set_at({'B', 7}, {PAWN, BLACK});
    board.set_at({'C', 7}, {PAWN, BLACK});
    board.set_at({'D', 7}, {PAWN, BLACK});
    board.set_at({'E', 7}, {PAWN, BLACK});
    board.set_at({'F', 7}, {PAWN, BLACK});
    board.set_at({'G', 7}, {PAWN, BLACK});
    board.set_at({'H', 7}, {PAWN, BLACK});
    board.set_at({'A', 8}, {ROOK, BLACK});
    board.set_at({'B', 8}, {KNIGHT, BLACK});
    board.set_at({'C', 8}, {BISHOP, BLACK});
    board.set_at({'D', 8}, {QUEEN, BLACK});
    board.set_at({'E', 8}, {KING, BLACK});
    board.set_at({'F', 8}, {BISHOP, BLACK});
    board.set_at({'G', 8}, {KNIGHT, BLACK});
    board.set_at({'H', 8}, {ROOK, BLACK});
    return board;
}


chess::Piece& chess::Board::piece_at(const Coord coord) {
    return this->board_state.at(coord.col).at(coord.row);
}

const chess::Piece& chess::Board::piece_at(const Coord coord) const {
    return this->board_state.at(coord.col).at(coord.row);
}

void chess::Board::set_at(const Coord coord, const Piece piece) {
    piece_at(coord) = piece;
}
