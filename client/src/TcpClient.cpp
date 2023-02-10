#include "TcpClient.h"

app::TcpClient::TcpClient(std::string ip, size_t port)
    : ep(asio::ip::address::from_string(ip), port), tcpSocket(io) {
    set_level(spdlog::level::debug);
    connect();
}

void app::TcpClient::connect() {
    tcpSocket.async_connect(ep, [this](const asio::system_error &e) {
        connectFlag = true;
        debug("connect success");
    });
    io.run();
}

void app::TcpClient::Process(const ProtoBuf protobuf) {
    debug("new Process");
    std::shared_ptr<asio::streambuf> buf = std::make_shared<asio::streambuf>();
    std::shared_ptr<std::ostream> os =
        std::make_shared<std::ostream>(buf.get());
    *os << protobuf;
    debug("Send: {} {} {}", ProtoBuf::MethodToString(protobuf.GetMethod()),
          protobuf.GetPath().string(), protobuf.GetData());

    // NOTE: async_write
    asio::async_write(
        tcpSocket, *buf, [this](const asio::error_code e, size_t size) {
            if (e) throw e;
            debug("Send success");

            std::shared_ptr<asio::streambuf> resultBuf =
                std::make_shared<asio::streambuf>();
            asio::async_read_until(
                tcpSocket, *resultBuf, '\n',
                [resultBuf, this](const asio::error_code &e, size_t size) {
                    debug("Read success");
                    std::istream is(resultBuf.get());
                    std::getline(is, result);
                });
        });
    try {
        io.reset();
        io.run();
    } catch (asio::system_error &e) {
        throw e;
    }
}

void app::TcpClient::handleGet(const std::filesystem::path &path) {
    if (path.empty()) {
        throw std::runtime_error("path is empty");
    }
    Process({ProtoBuf::Method::Get, path, "null"});
}

void app::TcpClient::handlePost(const std::filesystem::path &path,
                                       const std::string data) {
    if (path.empty()) {
        throw std::runtime_error("path is empty");
    }
    Process({ProtoBuf::Method::Post, path, data});
}

void app::TcpClient::handleDelete(const std::filesystem::path &path) {
    if (path.empty()) {
        throw std::runtime_error("path is empty");
    }
    Process({ProtoBuf::Method::Delete, path, "null"});
};
