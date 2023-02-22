#pragma once

#include <asio.hpp>
#include <csignal>
#include <filesystem>
#include <memory>
#include <variant>
#include <vector>

#include "ProtoBuf.h"
#include "WriteSession.h"

class TcpServer : public std::enable_shared_from_this<TcpServer> {
    asio::io_context io;
    asio::ip::tcp::acceptor acceptor{io};
    asio::signal_set sig{io, SIGINT, SIGTERM};

    std::string ip = "127.0.0.1";
    std::size_t port = 8000;
    std::string level = "info";
    std::size_t filesplit = 65536 * 4;
    std::size_t threads;
    /**
     * handle close socket
     */
    void handleCloseSocket(std::shared_ptr<asio::ip::tcp::socket>);

    /**
     * handle File Action
     */
    auto handleFileAction(ProtoBuf &)
        -> std::variant<std::string, std::vector<std::vector<char>>>;

    /**
     * handle read
     */
    void handleRead(std::shared_ptr<asio::ip::tcp::socket>,std::shared_ptr<WriteSession>);

   public:
    TcpServer() = default;
    TcpServer(const TcpServer &) = delete;
    TcpServer(TcpServer &&) = delete;
    TcpServer &operator=(const TcpServer &) = delete;
    TcpServer &operator=(TcpServer &&) = delete;

    [[nodiscard]] std::string getIp() const;
    void setIp(const std::string &);
    [[nodiscard]] std::size_t getPort() const;
    void setPort(const std::size_t &);
    [[nodiscard]] std::string getLevel() const;
    void setLevel(const std::string &);
    [[nodiscard]] std::size_t getFilesplitsize() const;
    void setFilesplit(const std::size_t &);
    [[nodiscard]] std::size_t getThreads() const;
    void setThreads(const std::size_t &);

    /**
     * handle signal
     */
    void handleSignal();
    /**
     * handle accept
     */
    void handleAccept();

    /**
     * run
     */
    void run();
};
