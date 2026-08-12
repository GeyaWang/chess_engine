#include <chess/board_state.hpp>


int main() {
    using namespace mf::chess;

    auto board_state = BoardState::create_default();
    BoardState::draw(board_state);

    board_state.set_at(WHITE_PAWN, square<'b', 5>());
    board_state.set_at(BLACK_KING, square<'h', 6>());
    BoardState::draw(board_state);
}
