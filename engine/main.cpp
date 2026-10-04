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
        return std::nullopt;
    }

    const mf::chess::Square from = parse_square(s[0], s[1]);
    const mf::chess::Square to = parse_square(s[2], s[3]);
    if (from == ERR_SQUARE || to == ERR_SQUARE) {
        return std::nullopt;
    }

    const mf::chess::PieceType promo = s.size() == 5 ? parse_piece(s[4]) : mf::chess::NONE;

    return ParsedMove{from, to, promo};
}

std::string move_to_uci(const mf::chess::Move& move) {
    return square_to_str(move.from) + square_to_str(move.to) + piece_to_str(move.promotion);
}


int main() {
    mf::chess::Game game{};

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        const auto str_list = parse_msg(msg);

        if (const auto& cmd = str_list.at(0); cmd == "uci") {
            std::cout << "id name Mockfish\n" << "id author Geya Wang\n" << "uciok" << std::endl;
        }
        else if (cmd == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (cmd == "ucinewgame") {
            game.restart();
        }
        else if (cmd == "position") {
            size_t i = 1;
            if (i < str_list.size() && str_list[i] == "startpos") {
                game.restart();
                i++;
            } else if (i < str_list.size() && str_list[i] == "fen") {
                std::string fen;
                for (i++; i < str_list.size() && str_list[i] != "moves"; i++) fen += str_list[i] + " ";
                game.set_fen(fen);
            }
            if (i < str_list.size() && str_list[i] == "moves") {
                for (i++; i < str_list.size(); i++) {
                    auto parsed_move = parse_uci_move(str_list[i]);
                    if (!parsed_move.has_value()) {
                        std::cerr << "Invalid move from client, bad uci, msg: '" << msg << "'\n";
                        std::cout << "error" << std::endl;
                        continue;
                    }

                    const auto [from_square, to_square, promotion] = parsed_move.value();
                    if (const bool is_valid_move = game.make_move(from_square, to_square, promotion); !is_valid_move) {
                        std::cerr << "Invalid move from client, illegal move, msg: '" << msg << "'\n";
                        std::cout << "error" << std::endl;
                    }
                }
            }
        }
        else if (cmd == "go") {
            const auto [nodes_searched, best_move] = mf::engine::Search::best_move(game, 5);
            if (best_move.piece == mf::chess::NONE) {
                std::cerr << "No legal moves found, msg: '" << msg << "'\n";
                std::cout << "error" << std::endl;
                continue;
            }
            std::cout << "bestmove " << move_to_uci(best_move) << "\n";
        }
        else if (cmd == "quit") {
            break;
        }
        else {
            std::cerr << "Unknown command: '" << msg << "'\n";
            std::cout << "error" << std::endl;
        }
    }

    return 0;
}
