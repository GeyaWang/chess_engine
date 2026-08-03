#include <chess/render.hpp>
#include <iostream>


std::string chess::Render::get_piece_symbol(const Piece &piece) {
    if (piece.colour == BLACK) {
        switch (piece.piece_type) {
            case EMPTY:
                return " ";
            case PAWN:
                return "♙";
            case KNIGHT:
                return "♘";
            case BISHOP:
                return "♗";
            case ROOK:
                return "♖";
            case QUEEN:
                return "♕";
            case KING:
                return "♔";
        }
    }

    // Colour == WHITE
    switch (piece.piece_type) {
        case EMPTY:
            return " ";
        case PAWN:
            return "♟";
        case KNIGHT:
            return "♞";
        case BISHOP:
            return "♝";
        case ROOK:
            return "♜";
        case QUEEN:
            return "♛";
        case KING:
            return "♚";
    }

    throw std::invalid_argument("Unknown piece type given");
}


std::string repeat_string(const std::string& s, const int n) {
    std::string result;
    for (int i = 0; i < n; i++) {
        result += s;
    }
    return result;
}


void chess::Render::draw_board(const Board &board) {
    static std::string BOARD_TOP = "  ┌─" + repeat_string("──┬─", 7) + "──┐";
    static std::string BOARD_MID = "  ├─" + repeat_string("──┼─", 7) + "──┤";
    static std::string BOARD_BOT = "  └─" + repeat_string("──┴─", 7) + "──┘";
    static std::string BOTTOM = "    A   B   C   D   E   F   G   H";

    system("clear");
    std::cout << BOARD_TOP << "\n";
    for (int r = 7; r >= 0; r--) {
        std::cout << r << " │ ";
        for (int c = 0; c < 8; c++) {
            std::cout << get_piece_symbol(board.piece_at({c, r})) << " │ ";
        }
        if (r != 0) {
            std::cout << "\n" << BOARD_MID << "\n";
        }
    }
    std::cout << "\n" << BOARD_BOT << "\n";
    std::cout << BOTTOM << "\n";
}
