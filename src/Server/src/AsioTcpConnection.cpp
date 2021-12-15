/*
** EPITECH PROJECT, 2021
** NetworkCommunication
** File description:
** AsioTcpConnection
*/

#include "../include/AsioTcpConnection.hpp"

using nlohmann::json;

es::server::AsioTcpConnection::AsioTcpConnection(boost::asio::ip::tcp::socket &sock): _connected(true) ,_socket(std::move(sock))
{
}

es::server::AsioTcpConnection::~AsioTcpConnection()
{
}

void es::server::AsioTcpConnection::readMessage()
{
    if (!_socket.is_open()) {
        std::cout << "socket is closed" << std::endl;
        return;
    }

    _socket.async_read_some(boost::asio::buffer(_buffer, MSGMAX), [this](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            std::string msg(_buffer);
            json tmp = json::parse(msg);
            _messages.push_back(tmp);
            std::cout << "[SERVER EASYSTREAM]: message received: " << tmp << std::endl;
        } else if (ec == boost::asio::error::eof) {
            std::cout << "[SERVER EASYSTREAM]: Socket has been disconnected" << std::endl;
            _connected = false;
        }
        else {
            std::cout << "[SERVER EASYSTREAM]: ERROR READING MESSAGE: " << ec.message() << std::endl;
            _connected = false;
        }
        readMessage();
    });
}

void es::server::AsioTcpConnection::writeMessage(const std::string &msg)
{
    char buffer[MSGMAX];

    std::memset(buffer, 0, MSGMAX);
    std::memcpy(buffer, msg.data(), msg.size());
    boost::asio::async_write(_socket, boost::asio::buffer(buffer, MSGMAX), [this](boost::system::error_code ec, std::size_t length) {
        (void)length;
        if (!ec)
            std::cout << "[SERVER EASYSTREAM]: Message sent to " << _socket.remote_endpoint() << std::endl;
        else {
            std::cout << "[SERVER EASYSTREAM]: Write failed: " << ec.message() << std::endl;
            _connected = false;
            // _socket.close();
        }
    });
}

const boost::asio::ip::tcp::socket &es::server::AsioTcpConnection::getSocket() const
{
    return (_socket);
}

const std::vector<nlohmann::json> es::server::AsioTcpConnection::getMessage()
{
    std::vector<nlohmann::json> tmp = _messages;

    _messages.clear();
    return (tmp);
}

bool es::server::AsioTcpConnection::isConnected() const
{
    return (_connected);
}