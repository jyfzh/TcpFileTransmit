// FIXME: 不能分离编译
#pragma once
#include <spdlog/spdlog.h>

#include <array>
#include <asio.hpp>
#include <filesystem>
#include <iostream>

#include "ProtoBuf.h"
/**
 * @brief TcpClient
 */
class TcpClient {
    asio::io_service io;
    asio::ip::tcp::endpoint ep /*(asio::ip::address::from_string("127.0.0.1"),
                                1234)*/
        ;
    asio::ip::tcp::socket sock /*(io)*/;
    ProtoBuf pb;

   public:
    std::string handleGet(const std::filesystem::path &path);
    template <typename T>
    std::string handlePost(const std::filesystem::path &path, T &&data);
    std::string handleDelete(const std::filesystem::path &path);

    TcpClient(std::string ip, size_t port);
};

TcpClient::TcpClient(std::string ip, size_t port)
    : ep(asio::ip::address::from_string(ip), port), sock(io) {
    using namespace spdlog;
    // NOTE: set_level
    set_level(spdlog::level::debug);
    sock.connect(ep);
    info("Connect success");
}

std::string TcpClient::handleGet(const std::filesystem::path &path) {
    using namespace spdlog;
    // NOTE: protoBuf
    pb.SetMethod(ProtoBuf::Method::Get);
    pb.SetPath(path);

    debug("path: {}", pb.GetPath().string());

    sock.write_some(asio::buffer(pb.GetProtoBuf<std::array<char, SIZE>>()));
    info("Send success");

    std::array<char, SIZE> buf;
    size_t size = sock.read_some(asio::buffer(buf));
    info("Receive success {} byte", size);
    return {buf.data(), size};
}

template <typename T>
std::string TcpClient::handlePost(const std::filesystem::path &path, T &&data) {
    using namespace spdlog;
    // TODO: protoBuf
    pb.SetMethod(ProtoBuf::Method::Post);
    pb.SetPath(path);

    debug("path: {}", pb.GetPath().string());
    pb.SetData(data);

    sock.write_some(asio::buffer(pb.GetProtoBuf<std::array<char, SIZE>>()));
    info("Send success");

    std::array<char, SIZE> buf;
    size_t size = sock.read_some(asio::buffer(buf));
    info("Receive success {} byte", size);
    return {buf.data(), size};
}

std::string TcpClient::handleDelete(const std::filesystem::path &path) {
    using namespace spdlog;
    // TODO: protoBuf
    pb.SetMethod(ProtoBuf::Method::Delete);
    pb.SetPath(path);

    debug("path: {}", pb.GetPath().string());

    sock.write_some(asio::buffer(pb.GetProtoBuf<std::array<char, SIZE>>()));
    info("Send success");

    std::array<char, SIZE> buf;
    size_t size = sock.read_some(asio::buffer(buf));
    info("Receive success {} byte", size);
    return {buf.data(), size};
};
