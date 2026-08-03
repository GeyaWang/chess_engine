#include <utils/parser.hpp>


bool utils::Parser::eof() const {
    return pos >= input.size();
}

char utils::Parser::peek() const {
    return eof() ? '\0' : input[pos];
}

char utils::Parser::get() {
    if (eof())
        throw std::runtime_error("Unexpected end of input");
    return input[pos++];
}

bool utils::Parser::consume(const char expected) {
    if (peek() == expected) {
        ++pos;
        return true;
    }
    return false;
}

void utils::Parser::expect(const char expected) {
    if (!consume(expected)) {
        throw std::runtime_error(
            "Expected '" + std::string(1, expected) + "' but found '" + std::string(1, peek()) + "'"
        );
    }
}
