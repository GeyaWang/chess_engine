#pragma once

#include <array>
#include <chess/board_state.hpp>
#include <chess/move.hpp>
#include <string>


namespace mf::chess {
    class Game {
        static constexpr int MAX_MOVES = 1000;

        std::array<BoardState, MAX_MOVES> state_history_;
        int current_index_ = 0;

    public:
        Game();

        BoardState& get_current_board();

        void make_move(const Move& move);
        void make_move(const std::string& from, const std::string& to, Type promotion);
        void undo_move();
    };
}
