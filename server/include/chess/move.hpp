#pragma once

#include <chess/piece.hpp>
#include <chess/coord.hpp>
#include <string>


namespace chess {
    struct Move {
        PieceType piece_type;
        Coord coord1;
        Coord coord2;

        static Move parse(const std::string& s);
    };
}
