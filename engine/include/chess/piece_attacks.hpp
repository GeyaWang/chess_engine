#pragma once

#include <array>
#include <chess/board_state.hpp>


namespace mf::chess {
    using Attacks = std::array<Board, 64>;

    constexpr bool coord_in_bounds(const int x, const int y) {
        return 0 <= x && x < 8 && 0 <= y && y < 8;
    }

    constexpr Square get_square(const int x, const int y) {
        return x + 8 * y;
    }

    template<int n>
    constexpr Attacks generate_stepping_moves(const std::array<std::pair<int, int>, n> moves) {
        Attacks boards{};
        for (int x1 = 0; x1 < 8; x1++) {
            for (int y1 = 0; y1 < 8; y1++) {

                for (const auto [dx, dy] : moves) {
                    const int x2 = x1 + dx;
                    const int y2 = y1 + dy;

                    if (coord_in_bounds(x2, y2)) {
                        boards[get_square(x1, y1)] |= static_cast<uint64_t>(1) << get_square(x2, y2);
                    }
                }

            }
        }
        return boards;
    }

    constexpr std::array<Attacks, 4> generate_sliding_moves(const std::array<std::pair<int, int>, 4>& directions) {
        std::array<Attacks, 4> boards{};

        for (int i = 0; i < 4; i++) {
            const auto [dx, dy] = directions[i];

            for (int x1 = 0; x1 < 8; x1++) {
                for (int y1 = 0; y1 < 8; y1++) {

                    int x2 = x1 + dx;
                    int y2 = y1 + dy;

                    while (coord_in_bounds(x2, y2)) {
                        boards[i][get_square(x1, y1)] |= static_cast<uint64_t>(1) << get_square(x2, y2);
                        x2 = x2 + dx;
                        y2 = y2 + dy;
                    }
                }
            }
        }

        return boards;
    }


    enum DiagonalDirection : uint8_t {
        NORTHWEST,
        SOUTHWEST,
        NORTHEAST,
        SOUTHEAST
    };

    enum CardinalDirection : uint8_t {
        WEST,
        NORTH,
        SOUTH,
        EAST
    };

    constexpr std::array DIAGONAL_DIRECTIONS = {NORTHWEST, SOUTHWEST, NORTHEAST, SOUTHEAST};
    constexpr std::array CARDINAL_DIRECTIONS = {WEST, NORTH, SOUTH, EAST};


    static constexpr Attacks KNIGHT_ATTACKS = generate_stepping_moves<8>(
        {
            std::pair{-2, -1},
            std::pair{-2,  1},
            std::pair{-1, -2},
            std::pair{-1,  2},
            std::pair{ 1, -2},
            std::pair{ 1,  2},
            std::pair{ 2, -1},
            std::pair{ 2,  1}
        }
    );
    static constexpr Attacks KING_ATTACKS = generate_stepping_moves<8>(
        {
            std::pair{-1, -1},
            std::pair{-1,  0},
            std::pair{-1, 1},
            std::pair{0,  -1},
            std::pair{ 0, 1},
            std::pair{ 1,  -1},
            std::pair{ 1, 0},
            std::pair{ 1,  1}
        }
    );
    static constexpr std::array<Attacks, 4> DIAGONAL_ATTACKS = generate_sliding_moves(
        {
            std::pair{-1, -1},
            std::pair{-1, 1},
            std::pair{1, -1},
            std::pair{1, 1}
        }
    );
    static constexpr std::array<Attacks, 4> CARDINAL_ATTACKS = generate_sliding_moves(
        {
            std::pair{-1, 0},
            std::pair{0, -1},
            std::pair{0, 1},
            std::pair{1, 0}
        }
    );
}
