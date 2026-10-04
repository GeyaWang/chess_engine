#pragma once

#include <cstdint>
#include <random>
#include <chess/game.hpp>


namespace mf::engine {
    struct SearchMove {
        uint64_t nodes_searched{};
        chess::Move move;
    };


    class Search {
    public:
        static auto get_gen() {
            std::random_device rd;
            const std::mt19937 gen(rd());
            return gen;
        }

        Search() : gen_(
            []{
                std::random_device rd;
                return std::mt19937(rd());
            }()
        ) {}
        static SearchMove best_move(chess::Game& game, int depth);

    private:
        std::mt19937 gen_;

        static int evaluate(const chess::BoardState& board_state);
        static int minimax(uint64_t& nodes_searched, chess::Game& game, int depth, int alpha, int beta);
    };
}
