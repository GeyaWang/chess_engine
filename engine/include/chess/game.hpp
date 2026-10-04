#pragma once

#include <chess/board_state.hpp>
#include <chess/move.hpp>
#include <chess/zobrist_hash.hpp>
#include <string>
#include <array>


namespace mf::chess {
    enum TerminalState : uint8_t {
        WHITE_WIN,
        BLACK_WIN,
        DRAW,
        NON_TERMINAL,
    };


    class Game {
        static constexpr int MAX_MOVES = 1000;

        std::array<BoardState, MAX_MOVES> state_history_;
        int current_index_ = 0;

        HashGenerator hash_generator_{};

    public:
        Game();

        BoardState& get_current_board();

        void make_move(const Move& move);
        bool make_move(Square from, Square to, PieceType promotion);
        void undo_move();
        void restart();
        void set_fen(const std::string& fen);

        bool is_soft_draw();
        TerminalState get_terminal_state();
    };
}
