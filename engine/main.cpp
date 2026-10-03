#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <chess/game.hpp>
#include <engine/search.hpp>


constexpr mf::chess::Square ERR_SQUARE = 64;


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


mf::chess::Square parse_square(const char file, const char rank) {
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
        return ERR_SQUARE;
    }

    const uint8_t x = file - 'a';
    const uint8_t y = 7 - (rank - '1');
    return x + y * 8;
}


mf::chess::PieceType parse_piece(const char c) {
    if (c == 'n') return mf::chess::WHITE_KNIGHT;
    if (c == 'b') return mf::chess::WHITE_BISHOP;
    if (c == 'r') return mf::chess::WHITE_ROOK;
    if (c == 'q') return mf::chess::WHITE_QUEEN;
    if (c == 'k') return mf::chess::WHITE_KING;
    return mf::chess::NONE;
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
        default: return "";
    }
}


std::string square_to_str(const mf::chess::Square square) {
    if (square < 0 || square >= 64) {
        return "";
    }

    const uint8_t x = square % 8;
    const uint8_t y = square / 8;

    return std::string{
        static_cast<char>('a' + x), static_cast<char>('8' - y)
    };
}


struct ParsedMove {
    mf::chess::Square from_square;
    mf::chess::Square to_square;
    mf::chess::PieceType promotion;
};


std::optional<ParsedMove> parse_uci_move(const std::string& s) {
    if (s.size() < 4 || s.size() > 5) {
        throw std::invalid_argument("Invalid uci move");
    }

    const mf::chess::Square from = parse_square(s[0], s[1]);
    const mf::chess::Square to = parse_square(s[2], s[3]);
    if (from == ERR_SQUARE || to == ERR_SQUARE) {
        return std::nullopt;
    }

    const mf::chess::PieceType promo = s.size() == 5 ? parse_piece(s[4]) : mf::chess::NONE;
    return ParsedMove{from, to, promo};
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
            if (string_list.size() <= 1) {
                std::cout << "ERROR Invalid move by client, msg: '" << msg << "'\n";
            }

            const auto parsed_move = parse_uci_move(string_list.at(1));
            if (!parsed_move.has_value()) {
                std::cout << "ERROR Invalid move by client, msg: '" << msg << "'\n";
                continue;
            }

            const auto [from_square, to_square, promotion] = parsed_move.value();
            if (const bool is_valid_move = game.make_move(from_square, to_square, promotion); !is_valid_move) {
                std::cout << "ERROR Invalid move by client, msg: '" << msg << "'\n";
                continue;
            }

            const auto [nodes_searched, best_move] = search.best_move(game, 4);
            game.make_move(best_move);
            std::cout << "bestmove " << square_to_str(best_move.from) << square_to_str(best_move.to) << piece_to_str(best_move.promotion) << "\n";
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
