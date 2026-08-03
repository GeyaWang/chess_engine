#include <chess/coord.hpp>


bool chess::Coord::is_valid_axis(const int x) {
    return 0 <= x && x <= 8;
}

int chess::Coord::column_char_to_int(const char c) {
    return static_cast<int>(c) - 65;
}

int chess::Coord::column_int_to_char(const int c) {
    return static_cast<char>(c + 65);
}
