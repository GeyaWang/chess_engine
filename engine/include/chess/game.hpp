#pragma once

#include <string>
#include <array>
#include <stdexcept>
#include <chess/board_state.hpp>


namespace mf::chess {
    static uint8_t parse(const std::string& s) {
        if (s.length() != 2) {
            throw std::invalid_argument("Algebraic notation must be given");
        }
        const uint8_t x = s[0] - 'a';
        const uint8_t y = 7 - (s[1] - '1');
        if (x < 0 || x >= 8 || y < 0 || y >= 8) {
            throw std::invalid_argument("Algebraic notation must be given");
        }
        return y * 8 + x;
    }


    struct Move {
        uint8_t from;
        uint8_t to;

        Move() = default;
        Move(const uint8_t from, const uint8_t to) : from(from), to(to) {}
        Move(const std::string& s1, const std::string& s2) : from(parse(s1)), to(parse(s2)) {}

        bool operator==(const Move& rhs) const { return from == rhs.from && to == rhs.to; }
    };


    struct MoveList {
        std::array<Move, 256> moves{};
        int count = 0;
        void add(const Move m) { moves[count++] = m; }
    };


    class Game {
        std::array<BoardState, 1000> board_states_;
        int current_index_ = 0;

        void gen_piece_moves(MoveList& move_list, PieceType piece_type, uint8_t pos);

    public:
        Game();

        BoardState& get_current_board();

        void move(const Move& move);
        void undo();

        MoveList gen_moves();
        bool is_legal(const Move& move);
    };
}
