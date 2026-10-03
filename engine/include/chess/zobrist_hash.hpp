#pragma once

#include <cstdint>
#include <array>
#include <random>
#include <chess/board_state.hpp>


namespace mf::chess {
    class HashGenerator {
    public:
        [[nodiscard]] Hash hash(const BoardState& board_state) const;
        HashGenerator();

    private:
        std::random_device rd_;
        std::mt19937_64 gen_;
        std::uniform_int_distribution<uint64_t> dist_;

        template<size_t n>
        std::array<Hash, n> gen_hashes();

        const std::array<Hash, 768> board_hashes;
        const Hash turn_hash;
        const std::array<Hash, 4> castling_hashes;
        const std::array<Hash, 64> en_passent_hashes;
    };
}
