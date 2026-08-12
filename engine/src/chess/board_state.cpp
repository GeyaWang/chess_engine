#include <chess/board_state.hpp>
#include <string>
#include <iostream>


namespace {
    using namespace mf::chess;
    std::string get_piece_symbol(const PieceType piece) {
        switch (piece) {
            case EMPTY:
                return " ";
            case BLACK_PAWN:
                return "♙";
            case BLACK_KNIGHT:
                return "♘";
            case BLACK_BISHOP:
                return "♗";
            case BLACK_ROOK:
                return "♖";
            case BLACK_QUEEN:
                return "♕";
            case BLACK_KING:
                return "♔";
            case WHITE_PAWN:
                return "♟";
            case WHITE_KNIGHT:
                return "♞";
            case WHITE_BISHOP:
                return "♝";
            case WHITE_ROOK:
                return "♜";
            case WHITE_QUEEN:
                return "♛";
            default:  // BLACK_KING
                return "♚";;
        }
    }

    std::string repeat_string(const std::string& s, const int n) {
        std::string result;
        for (int i = 0; i < n; i++) {
            result += s;
        }
        return result;
    }
}


namespace mf::chess {
    void BoardState::draw(const BoardState& board_state) {
        static std::string BOARD_TOP = "  ┌─" + repeat_string("──┬─", 7) + "──┐";
        static std::string BOARD_MID = "  ├─" + repeat_string("──┼─", 7) + "──┤";
        static std::string BOARD_BOT = "  └─" + repeat_string("──┴─", 7) + "──┘";
        static std::string BOTTOM = "    A   B   C   D   E   F   G   H";

        std::cout << BOARD_TOP << "\n";
        for (int r = 8; r >= 1; r--) {
            std::cout << r << " │ ";
            for (int c = 0; c < 8; c++) {
                const int pos = (8 - r) * 8 + c;
                std::cout << get_piece_symbol(board_state.piece_at(pos)) << " │ ";
            }
            if (r != 1) {
                std::cout << "\n" << BOARD_MID << "\n";
            }
        }
        std::cout << "\n" << BOARD_BOT << "\n";
        std::cout << BOTTOM << "\n";
    }

    BoardState BoardState::create_default() {
        BoardState default_board{};
        default_board.bitboards[WHITE_PAWN]   = 0x00ff000000000000;
        default_board.bitboards[WHITE_KNIGHT] = 0x4200000000000000;
        default_board.bitboards[WHITE_BISHOP] = 0x2400000000000000;
        default_board.bitboards[WHITE_ROOK]   = 0x8100000000000000;
        default_board.bitboards[WHITE_QUEEN]  = 0x0800000000000000;
        default_board.bitboards[WHITE_KING]   = 0x1000000000000000;
        default_board.bitboards[BLACK_PAWN]   = 0x000000000000ff00;
        default_board.bitboards[BLACK_KNIGHT] = 0x0000000000000042;
        default_board.bitboards[BLACK_BISHOP] = 0x0000000000000024;
        default_board.bitboards[BLACK_ROOK]   = 0x0000000000000081;
        default_board.bitboards[BLACK_QUEEN]  = 0x0000000000000008;
        default_board.bitboards[BLACK_KING]   = 0x0000000000000010;
        return default_board;
    }

    PieceType BoardState::piece_at(const uint8_t pos) const {
        const uint64_t mask = static_cast<uint64_t>(1) << pos;
        for (const auto i : ALL_PIECES) {
            if (bitboards[i] & mask) {
                return i;
            }
        }
        return EMPTY;
    }

    void BoardState::set_at(const PieceType piece, const uint8_t pos) {
        const uint64_t mask = static_cast<uint64_t>(1) << pos;
        for (const auto i : ALL_PIECES) {
            if (i == piece) {
                bitboards[i] |= mask;
            } else {
                bitboards[i] &= ~mask;
            }
        }
    }
}
