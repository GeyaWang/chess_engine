#include <network/session.hpp>


namespace mf::network {
    std::error_code Session::start() {
        try {
            acceptor_.accept(socket_);
        } catch (std::error_code& err) {
            return err;
        }
        return std::error_code{};
    }

    void Session::close() {
        socket_.close();
    }

    bool Session::is_open() const {
        return socket_.is_open();
    }

    std::error_code Session::write(const std::string &msg) {
        std::error_code ec;
        asio::write(socket_, asio::buffer(msg), ec);
        return ec;
    }

    ListenResult Session::listen() {
        ListenResult result;
        char data[1024];
        const std::size_t bytes_received = socket_.read_some(asio::buffer(data), result.error_code);
        if (!result.error_code) {
            result.msg = std::string(data, bytes_received);
        }
        return result;
    }

    ListenResult Session::listen_until(const std::string& s) {
        ListenResult result;
        asio::read_until(socket_, buffer_, s, result.error_code);
        if (!result.error_code) {
            std::istream is(&buffer_);
            std::string str;
            std::getline(is, str);
            buffer_.consume(buffer_.size());
            result.msg = str;
        }
        return result;
    }
}
