#include <chess/board_state.hpp>


constexpr uint8_t square(const char col, const int row) {
    return (8 - row) * 8 + (col - 'a');
}


int main() {
    using namespace mf::chess;

    auto board_state = BoardState::create_default();
    draw_board(board_state);

    board_state.set_at(WHITE_PAWN, square('b', 5));
    board_state.set_at(BLACK_KING, square('h', 6));
    draw_board(board_state);
}
