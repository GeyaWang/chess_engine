#include <chess/game.hpp>
#include <sstream>
#include <chess/move_generator.hpp>
#include <utils/bit_operations.hpp>


namespace mf::chess {
    Game::Game() {
        state_history_[0] = BoardState::create_default();
    }

    BoardState &Game::get_current_board() {
        return state_history_[current_index_];
    }

    bool Game::make_move(const Square from, const Square to, const PieceType promotion) {
        std::array<Move, 218> moves{};
        const int count = MoveGenerator::gen_pseudo_legal(get_current_board(), moves);
        for (int i = 0; i < count; i++) {
            if (const Move move = moves[i]; move.from == from && move.to == to && move.promotion == promotion) {
                make_move(move);
                return true;
            }
        }
        return false;
    }

    void Game::make_move(const Move &move) {
        const BoardState& old_state = state_history_[current_index_];
        BoardState& new_state = state_history_[current_index_ + 1];
        new_state = old_state;

        const uint64_t from_board = bit(move.from);
        const uint64_t to_board = bit(move.to);
        const uint64_t move_board = from_board | to_board;

        const Colour ally_clr = get_piece_colour(move.piece);
        const int ally_pawn_dir = ally_clr == WHITE ? WHITE_PAWN_DIRECTION : BLACK_PAWN_DIRECTION;

        if (move.move_type & CAPTURE) {
            if (move.move_type & EN_PASSENT) {
                const u_int8_t capture_square = move.to - ally_pawn_dir;
                new_state.apply_mask(move.captured, bit(capture_square));
            } else {
                // Capture opponent piece
                new_state.apply_mask(move.captured, to_board);
            }
        }

        if (move.move_type & PROMOTION) {
            new_state.apply_mask(move.piece, from_board);
            new_state.apply_mask(move.promotion, to_board);
        } else {
            // Move piece
            new_state.apply_mask(move.piece, move_board);

            if (move.move_type & PAWN_DOUBLE) {
                // Create en-passent target
                new_state.en_passent_target = bit(move.from + ally_pawn_dir);
            } else {
                new_state.en_passent_target = 0;

                if (move.move_type & CASTLE) {
                    new_state.en_passent_target = 0;

                    // Make rook move
                    const uint8_t rook_from = move.to > move.from ? move.to + 1 : move.to - 2;
                    const uint8_t rook_to = move.to > move.from ? move.to - 1 : move.to + 1;
                    const uint64_t rook_move_board = bit(rook_from) | bit(rook_to);
                    const PieceType rook_type = ally_clr == WHITE ? WHITE_ROOK : BLACK_ROOK;
                    new_state.apply_mask(rook_type, rook_move_board);
                }
            }

            // Update castling rights
            if (move.piece == WHITE_KING) {
                new_state.castling_rights[WHITE] = NONE_CASTLE;
            } else if (move.piece == BLACK_KING) {
                new_state.castling_rights[BLACK] = NONE_CASTLE;
            } else if (move.piece == WHITE_ROOK) {
                if (old_state.castling_rights[WHITE] & KINGSIDE_CASTLE && move.from == sq('h', 1)) {
                    new_state.castling_rights[WHITE] &= ~KINGSIDE_CASTLE;
                } else if (QUEENSIDE_CASTLE & old_state.castling_rights[WHITE] && move.from == sq('a', 1)) {
                    new_state.castling_rights[WHITE] &= ~QUEENSIDE_CASTLE;
                }
            } else if (move.piece == BLACK_ROOK) {
                if (old_state.castling_rights[BLACK] & KINGSIDE_CASTLE && move.from == sq('h', 8)) {
                    new_state.castling_rights[BLACK] &= ~KINGSIDE_CASTLE;
                } else if (old_state.castling_rights[BLACK] & QUEENSIDE_CASTLE && move.from == sq('a', 8)) {
                    new_state.castling_rights[BLACK] &= ~QUEENSIDE_CASTLE;
                }
            }
        }

        new_state.hash = hash_generator_.hash(new_state);
        new_state.side_to_move = switch_colour(ally_clr);
        current_index_++;
    }

    void Game::undo_move() {
        if (current_index_ > 0) {
            current_index_--;
        }
    }

    void Game::restart() {
        current_index_ = 0;
        state_history_[0] = BoardState::create_default();
        state_history_[0].hash = hash_generator_.hash(state_history_[0]);
    }

    void Game::set_fen_pos(const std::string& fen_str) {
        BoardState board{};
        current_index_ = 0;
        state_history_[0] = board;

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
            char c = board_string[i];
            Square square = x + y * 8;

            if (c == '/') {
                x = 0;
                y++;
            } else if (c <= '9' && c >= '0') {
                x += c - '0';
            } else if (c == 'p') {
                board.set_at(BLACK_PAWN, square);
                x++;
            } else if (c == 'n') {
                board.set_at(BLACK_KNIGHT, square);
                x++;
            } else if (c == 'b') {
                board.set_at(BLACK_BISHOP, square);
                x++;
            } else if (c == 'r') {
                board.set_at(BLACK_ROOK, square);
                x++;
            } else if (c == 'q') {
                board.set_at(BLACK_QUEEN, square);
                x++;
            } else if (c == 'k') {
                board.set_at(BLACK_KING, square);
                x++;
            } else if (c == 'P') {
                board.set_at(WHITE_PAWN, square);
                x++;
            } else if (c == 'N') {
                board.set_at(WHITE_KNIGHT, square);
                x++;
            } else if (c == 'B') {
                board.set_at(WHITE_BISHOP, square);
                x++;
            } else if (c == 'R') {
                board.set_at(WHITE_ROOK, square);
                x++;
            } else if (c == 'Q') {
                board.set_at(WHITE_QUEEN, square);
                x++;
            } else if (c == 'K') {
                board.set_at(WHITE_KING, square);
                x++;
            }
        }

        // Turn
        if (turn == "b") {
            board.side_to_move = BLACK;
        } else {
            board.side_to_move = WHITE;
        }

        // Castling
        for (int i = 0; i < castling.length(); i++) {
            if (const char c = castling[i]; c == 'K') board.castling_rights[WHITE] |= KINGSIDE_CASTLE;
            else if (c == 'k') board.castling_rights[BLACK] |= KINGSIDE_CASTLE;
            else if (c == 'Q') board.castling_rights[WHITE] |= QUEENSIDE_CASTLE;
            else if (c == 'q') board.castling_rights[BLACK] |= QUEENSIDE_CASTLE;
        }

        // En-passent target
        if (enpassant_target.length() >= 2)
        {
            int passant_x = enpassant_target[0] - 'a';
            int passant_y = 7 - enpassant_target[1] + '1';

            uint8_t passantSquare = passant_x + 8 * passant_y;
            board.en_passent_target = bit(passantSquare);
        }

        // Hash
        board.hash = hash_generator_.hash(board);
    }

    TerminalState Game::get_terminal_state() {
        const BoardState& board_state = get_current_board();
        const Colour clr = board_state.side_to_move;

        std::array<Move, 218> moves{};
        const int move_count = MoveGenerator::gen_pseudo_legal(board_state, moves);

        for (int i = 0; i < move_count; i++) {
            make_move(moves[i]);
            const bool is_illegal = clr == WHITE ? get_current_board().is_king_attacked(WHITE) : get_current_board().is_king_attacked(BLACK);
            undo_move();

            if (!is_illegal) {
                return NON_TERMINAL;
            }
        }

        // State is terminal
        if (board_state.is_king_attacked(WHITE)) return BLACK_WIN;
        if (board_state.is_king_attacked(BLACK)) return WHITE_WIN;
        return DRAW;
    }
}
