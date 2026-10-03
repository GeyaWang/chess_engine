#include <iostream>
#include <string>
#include <vector>
#include <chess/game.hpp>
#include <engine/search.hpp>


std::vector<std::string> parse_msg(const std::string& msg) {
    std::vector<std::string> string_list;
    size_t idx = 0;
    while (true) {
        const auto del_idx = msg.find(' ', idx);
        if (del_idx == std::string::npos) {
            string_list.push_back(msg.substr(idx));   // rest of string
            break;
        }
        string_list.push_back(msg.substr(idx, del_idx - idx));
        idx = del_idx + 1;  // skip past the space
    }
    return string_list;
}


uint8_t parse_square(const std::string& s) {
    if (s.length() != 2) {
        throw std::invalid_argument("Algebraic notation must be given");
    }
    const uint8_t x = s[0] - 'a';
    const uint8_t y = 7 - (s[1] - '1');
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        throw std::invalid_argument("Algebraic notation must be given");
    }
    return y * 8 + x;
}


mf::chess::PieceType parse_piece(const std::string& s) {
    if (s == "n") return mf::chess::WHITE_KNIGHT;
    if (s == "b") return mf::chess::WHITE_BISHOP;
    if (s == "r") return mf::chess::WHITE_ROOK;
    if (s == "q") return mf::chess::WHITE_QUEEN;
    throw std::invalid_argument("Invalid piece str given");
}


std::string piece_to_str(const mf::chess::PieceType type) {
    switch (type) {
        case mf::chess::WHITE_PAWN:
        case mf::chess::BLACK_PAWN:
            return "p";
        case mf::chess::WHITE_KNIGHT:
        case mf::chess::BLACK_KNIGHT:
            return "n";
        case mf::chess::WHITE_BISHOP:
        case mf::chess::BLACK_BISHOP:
            return "b";
        case mf::chess::WHITE_ROOK:
        case mf::chess::BLACK_ROOK:
            return "r";
        case mf::chess::WHITE_QUEEN:
        case mf::chess::BLACK_QUEEN:
            return "q";
        case mf::chess::WHITE_KING:
        case mf::chess::BLACK_KING:
            return "k";
        default: throw std::invalid_argument("Invalid piece type given");
    }
}


std::string square_to_str(const uint8_t square) {
    const uint8_t x = square % 8;
    const uint8_t y = square / 8;

    return std::string{
        static_cast<char>('a' + x), static_cast<char>('8' - y)
    };
}


void gui_mode() {
    mf::chess::Game game{};
    mf::engine::Search search{};

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        const auto string_list = parse_msg(msg);

        const auto& prefix = string_list.at(0);
        if (prefix == "quit") {
            break;
        }
        if (prefix == "move") {
            const auto& pos1 = string_list.at(1);
            const auto& pos2 = string_list.at(2);

            if (string_list.size() >= 4) {
                const auto& promo = string_list.at(3);
                if (const bool is_valid_move = game.make_move(parse_square(pos1), parse_square(pos2), parse_piece(promo)); !is_valid_move) {
                    std::cout << "ERROR Invalid move by client, msg: '" << msg << "'\n";
                }
            } else {
                if (const bool is_valid_move = game.make_move(parse_square(pos1), parse_square(pos2), mf::chess::NONE); !is_valid_move) {
                    std::cout << "ERROR Invalid move by client, msg: '" << msg << "'\n";
                }
            }

            const auto [nodes_searched, best_move] = search.best_move(game, 4);
            game.make_move(best_move);
            if (best_move.promotion == mf::chess::NONE) {
                std::cout << "move " << square_to_str(best_move.from) << " " << square_to_str(best_move.to) << "\n";
            } else {
                std::cout << "move " << square_to_str(best_move.from) << " " << square_to_str(best_move.to) << " " << piece_to_str(best_move.promotion) << "\n";
            }
        }
        else {
            std::cout << "ERROR Unknown command: '" << msg << "'\n";
        }
    }
}


int main() {
    std::string command;
    std::getline(std::cin, command);
    if (command == "gui") {
        gui_mode();
    }
    return 0;
}
