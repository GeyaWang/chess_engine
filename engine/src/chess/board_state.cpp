#include <chess/board_state.hpp>
#include <string>
#include <iostream>


namespace mf::chess {
    namespace {
        std::string get_piece_symbol(const Type piece) {
            switch (piece) {
                case NONE:
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


    void draw_board(const BoardState& board_state) {
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
        BoardState b{};

        b.bitboards[WHITE_PAWN]   = bit(sq('a', 2)) | bit(sq('b', 2)) | bit(sq('c', 2)) | bit(sq('d', 2)) | bit(sq('e', 2)) | bit(sq('f', 2)) | bit(sq('g', 2)) | bit(sq('h', 2));
        b.bitboards[WHITE_KNIGHT] = bit(sq('b', 1)) | bit(sq('g', 1));
        b.bitboards[WHITE_BISHOP] = bit(sq('c', 1)) | bit(sq('f', 1));
        b.bitboards[WHITE_ROOK]   = bit(sq('a', 1)) | bit(sq('h', 1));
        b.bitboards[WHITE_QUEEN]  = bit(sq('d', 1));
        b.bitboards[WHITE_KING]   = bit(sq('e', 1));
        b.bitboards[BLACK_PAWN]   = bit(sq('a', 7)) | bit(sq('b', 7)) | bit(sq('c', 7)) | bit(sq('d', 7)) | bit(sq('e', 7)) | bit(sq('f', 7)) | bit(sq('g', 7)) | bit(sq('h', 7));
        b.bitboards[BLACK_KNIGHT] = bit(sq('b', 8)) | bit(sq('g', 8));
        b.bitboards[BLACK_BISHOP] = bit(sq('c', 8)) | bit(sq('f', 8));
        b.bitboards[BLACK_ROOK]   = bit(sq('a', 8)) | bit(sq('h', 8));
        b.bitboards[BLACK_QUEEN]  = bit(sq('d', 8));
        b.bitboards[BLACK_KING]   = bit(sq('e', 8));

        for (const auto& p : WHITE_PIECES) {
            b.occupancy[WHITE] |= b.bitboards[p];
        }
        for (const auto& p : BLACK_PIECES) {
            b.occupancy[BLACK] |= b.bitboards[p];
        }
        b.all_pieces = b.occupancy[WHITE] | b.occupancy[BLACK];

        b.castling_rights[WHITE] = KINGSIDE_CASTLE | QUEENSIDE_CASTLE;
        b.castling_rights[BLACK] = KINGSIDE_CASTLE | QUEENSIDE_CASTLE;

        return b;
    }

    Type BoardState::piece_at(const Square pos) const {
        const uint64_t mask = bit(pos);
        for (const auto i: ALL_PIECES) {
            if (bitboards[i] & mask) {
                return i;
            }
        }
        return NONE;
    }

    void BoardState::set_at(const Type piece, const Square pos) {
        const uint64_t mask = bit(pos);
        for (const auto i: ALL_PIECES) {
            if (i == piece) {
                bitboards[i] |= mask;
            } else {
                bitboards[i] &= ~mask;
            }
        }
    }

    void BoardState::apply_mask(const Type piece, const Board mask) {
        bitboards[piece] ^= mask;
        occupancy[get_piece_colour(piece)] ^= mask;
        all_pieces ^= mask;
    }
}
