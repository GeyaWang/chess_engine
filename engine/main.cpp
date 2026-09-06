#include <iostream>
#include <string>
#include <vector>
#include "chess/game.hpp"


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


void gui_mode() {
    mf::chess::Game game{};

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        const auto string_list = parse_msg(msg);

        const auto& prefix = string_list.at(0);
        if (prefix == "quit") {
            break;
        }
        // if (prefix == "move") {
        //     const auto& pos1 = string_list.at(1);
        //     const auto& pos2 = string_list.at(2);
        //     if (game.is_legal({pos1, pos2})) {
        //         game.make_move({pos1, pos2});
        //         std::cout << "ok\n";
        //     } else {
        //         std::cout << "bad\n";
        //     }
        // }
        else {
            std::cout << "Unknown command: '" << msg << "'\n";
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
