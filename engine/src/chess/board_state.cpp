#include <chess/board_state.hpp>
#include <chess/move_generator.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <utils/bit_operations.hpp>


namespace mf::chess {
    namespace {
        std::string get_piece_symbol(const PieceType piece) {
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

    PieceType BoardState::piece_at(const Square pos) const {
        const uint64_t mask = bit(pos);
        for (const auto i: ALL_PIECES) {
            if (bitboards[i] & mask) {
                return i;
            }
        }
        return NONE;
    }

    void BoardState::set_at(const PieceType piece, const Square pos) {
        const uint64_t mask = bit(pos);
        for (const auto i: ALL_PIECES) {
            if (i == piece) {
                bitboards[i] |= mask;
            } else {
                bitboards[i] &= ~mask;
            }
        }
    }

    void BoardState::apply_mask(const PieceType piece, const Board mask) {
        bitboards[piece] ^= mask;
        occupancy[get_piece_colour(piece)] ^= mask;
        all_pieces ^= mask;
    }

    void BoardState::set_fen(const std::string& fen_str) {
        std::stringstream ss;
        ss.str(fen_str);

        std::string board_string;
        std::string turn;
        std::string castling;
        std::string enpassant_target;
        std::string half_moves;
        std::string full_moves;

        ss >> board_string;
        ss >> turn;
        ss >> castling;
        ss >> enpassant_target;
        ss >> half_moves;
        ss >> full_moves;

        // Board
        int x = 0;
        int y = 0;
        for (int i = 0; i < board_string.length(); i++) {
            const char c = board_string[i];
            const Square square = x + y * 8;

            if (c == '/') {
                x = 0;
                y++;
            } else if (c <= '9' && c >= '0') {
                x += c - '0';
            } else if (c == 'p') {
                set_at(BLACK_PAWN, square);
                x++;
            } else if (c == 'n') {
                set_at(BLACK_KNIGHT, square);
                x++;
            } else if (c == 'b') {
                set_at(BLACK_BISHOP, square);
                x++;
            } else if (c == 'r') {
                set_at(BLACK_ROOK, square);
                x++;
            } else if (c == 'q') {
                set_at(BLACK_QUEEN, square);
                x++;
            } else if (c == 'k') {
                set_at(BLACK_KING, square);
                x++;
            } else if (c == 'P') {
                set_at(WHITE_PAWN, square);
                x++;
            } else if (c == 'N') {
                set_at(WHITE_KNIGHT, square);
                x++;
            } else if (c == 'B') {
                set_at(WHITE_BISHOP, square);
                x++;
            } else if (c == 'R') {
                set_at(WHITE_ROOK, square);
                x++;
            } else if (c == 'Q') {
                set_at(WHITE_QUEEN, square);
                x++;
            } else if (c == 'K') {
                set_at(WHITE_KING, square);
                x++;
            }
        }

        // Turn
        if (turn == "b") {
            side_to_move = BLACK;
        } else {
            side_to_move = WHITE;
        }

        // Castling
        for (int i = 0; i < castling.length(); i++) {
            if (const char c = castling[i]; c == 'K') castling_rights[WHITE] |= KINGSIDE_CASTLE;
            else if (c == 'k') castling_rights[BLACK] |= KINGSIDE_CASTLE;
            else if (c == 'Q') castling_rights[WHITE] |= QUEENSIDE_CASTLE;
            else if (c == 'q') castling_rights[BLACK] |= QUEENSIDE_CASTLE;
        }

        // En-passent target
        if (enpassant_target.length() >= 2)
        {
            const uint8_t passant_x = enpassant_target[0] - 'a';
            const uint8_t passant_y = 7 - (enpassant_target[1] - '1');

            const uint8_t passantSquare = passant_x + 8 * passant_y;
            en_passent_target = bit(passantSquare);
        }

        // Half moves
        fifty_move_rule_counter = std::stoi(half_moves);

        // Occupancy
        for (const auto& p : WHITE_PIECES) {
            occupancy[WHITE] |= bitboards[p];
        }
        for (const auto& p : BLACK_PIECES) {
            occupancy[BLACK] |= bitboards[p];
        }
        all_pieces = occupancy[WHITE] | occupancy[BLACK];
    }

    bool BoardState::is_king_attacked(const Colour colour) const {
        if (colour == WHITE) {
            return MoveGenerator::is_attacked<WHITE>(*this, utils::bit_index(bitboards[WHITE_KING]));
        }
        return MoveGenerator::is_attacked<BLACK>(*this, utils::bit_index(bitboards[BLACK_KING]));
    }
}
