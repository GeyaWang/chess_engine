#pragma once

#include <string>
#include <stdexcept>


namespace utils {
    class Parser {
        size_t pos = 0;
        const std::string& input;

    public:
        explicit Parser(const std::string& input) : input(input) {}

        [[nodiscard]] bool eof() const;
        [[nodiscard]] char peek() const;
        [[nodiscard]] char get();
        bool consume(char expected);
        void expect(char expected);

        [[nodiscard]] size_t position() const { return pos; }
    };
}
