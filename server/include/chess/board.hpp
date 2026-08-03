#pragma once

#include <array>
#include <chess/piece.hpp>
#include <chess/coord.hpp>


namespace chess {
    using BoardState = std::array<std::array<Piece, 8>, 8>;

    class Board {
        BoardState board_state{};
    public:
        Board() = default;

        static Board create_empty();
        static Board create_default();

        [[nodiscard]] Piece& piece_at(Coord coord);
        [[nodiscard]] const Piece& piece_at(Coord coord) const;

        void set_at(Coord coord, Piece piece);
    };
}
