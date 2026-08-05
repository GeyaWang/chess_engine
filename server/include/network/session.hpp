#pragma once

#include <asio.hpp>


namespace mf::network {
    using asio::ip::tcp;

    struct ListenResult {
        std::string msg;
        std::error_code error_code;
    };


    class Session {
        asio::io_context io_context_;
        tcp::socket socket_{io_context_};
        tcp::acceptor acceptor_;
        asio::streambuf buffer_;

    public:
        explicit Session(const std::string& addr, const uint16_t port) : acceptor_(io_context_, {asio::ip::make_address(addr), port}) {}

        std::error_code start();
        void close();

        [[nodiscard]] bool is_open() const;

        std::error_code write(const std::string& msg);
        ListenResult listen();
        ListenResult listen_until(const std::string& s);
    };
}
