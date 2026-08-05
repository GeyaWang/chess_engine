#include <iostream>
#include <network/session.hpp>
#include <asio.hpp>
#include <string>
#include <string_view>
#include <iostream>

const asio::ip::tcp::endpoint SERVER_ENDPOINT {
    asio::ip::make_address("127.0.0.1"),
    5000
};


void server() {
    std::cout << "Started server!\n";

    using asio::ip::tcp;
    asio::io_context ctx;
    tcp::acceptor acceptor(ctx, SERVER_ENDPOINT);
    tcp::socket socket(ctx);
    acceptor.accept(socket);

    char data[1024];
    socket.async_receive(
        asio::buffer(data, 1024),
        [&](const std::error_code ec, const size_t bytes_received) {
            if (!ec && bytes_received > 0) {
                std::cout << "RCV: " << std::string_view(data, bytes_received) << "\n";
            } else {
                std::cout << "Error! " << ec.message() << "\n";
            }
        }
    );

    ctx.run();
}


int main() {
    mf::network::Session s{"127.0.0.1", 5000};
    auto test = s.start();
    std::cout << test.message() << "\n";

    while (s.is_open()) {
        auto [msg, error_code] = s.listen();
        if (!error_code) {
            std::cout << "RCV: " << msg << "\n";
            s.write("Hi!!!");
        } else {
            std::cout << "Connection closed\n";
            s.close();
        }
    }
}
