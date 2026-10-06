#include <chess/zobrist_hash.hpp>
#include <utils/bit_operations.hpp>


namespace mf::chess {
    template<size_t n>
    std::array<Hash, n> HashGenerator::gen_hashes() {
        std::array<uint64_t, n> hashes{};

        for (int i = 0; i < n; i++) {
            hashes[i] = dist_(gen_);
        }
        return hashes;
    }


    HashGenerator::HashGenerator() : gen_(rd_()), dist_(0, std::numeric_limits<uint64_t>::max()),
        board_hashes(gen_hashes<768>()), turn_hash(dist_(gen_)), castling_hashes(gen_hashes<4>()), en_passent_hashes(gen_hashes<64>()) {}


    Hash HashGenerator::hash(const BoardState &board_state) const {
        Hash hash = 0;

        // Pieces
        for (const auto piece : ALL_PIECES) {
            uint64_t bitboard = board_state.bitboards[piece];
            while (bitboard) {
                hash ^= board_hashes[64 * piece + utils::lsb_index(bitboard)];
                utils::remove_lsb(bitboard);
            }
        }

        // Turn
        if (board_state.turn == BLACK) hash ^= turn_hash;

        // Castling Rights
        if (board_state.castling_rights[WHITE] & QUEENSIDE_CASTLE) hash ^= castling_hashes[0];
        if (board_state.castling_rights[WHITE] & KINGSIDE_CASTLE)  hash ^= castling_hashes[1];
        if (board_state.castling_rights[BLACK] & QUEENSIDE_CASTLE) hash ^= castling_hashes[2];
        if (board_state.castling_rights[BLACK] & KINGSIDE_CASTLE)  hash ^= castling_hashes[3];

        // En passent target
        if (board_state.en_passent_target) hash ^= en_passent_hashes[utils::bit_index(board_state.en_passent_target)];

        return hash;
    }
}
