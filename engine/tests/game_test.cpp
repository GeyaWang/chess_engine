#include <chess/game.hpp>
#include <iostream>


int main() {
    using namespace mf::chess;

    Game game{};

    const auto move_list = game.gen_moves();
    for (int i = 0; i < move_list.count; i++) {
        const auto move = move_list.moves[i];
        std::cout << "Move " << static_cast<int>(move.from) << " " << static_cast<int>(move.to) << "\n";
    }

    BoardState::draw(game.get_current_board());
    Move m {"c1", "a3"};
    std::cout << static_cast<int>(m.from) << " " << static_cast<int>(m.to) << "\n";
    std::cout << game.is_legal(m) << "\n";
    game.move(m);
    BoardState::draw(game.get_current_board());
}
