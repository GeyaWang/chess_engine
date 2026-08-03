#pragma once

#include <stdexcept>


namespace chess {
    class Coord {
        static int column_char_to_int(char c);
        static int column_int_to_char(int c);
        static bool is_valid_axis(int x);

    public:
        const int col;
        const int row;
        const int note_col;
        const int note_row;

        Coord(const int c, const int r) : col(c), row(r), note_col(column_int_to_char(c)), note_row(r+1) {
            if (!is_valid_axis(c) || !is_valid_axis(r)) {
                throw std::invalid_argument("Invalid coordinate given");
            }
        }
        Coord(const char c, const int r) : col(column_char_to_int(c)), row(r-1), note_col(c), note_row(r) {
            if (!is_valid_axis(column_char_to_int(c)) || !is_valid_axis(r-1)) {
                throw std::invalid_argument("Invalid coordinate given");
            }
        }


    };
}
