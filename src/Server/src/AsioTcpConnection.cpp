/*
** EPITECH PROJECT, 2021
** NetworkCommunication
** File description:
** AsioTcpConnection
*/

#include "../include/AsioTcpConnection.hpp"

using nlohmann::json;

es::server::AsioTcpConnection::AsioTcpConnection(boost::asio::ip::tcp::socket &sock): _socket(std::move(sock))
{
}

es::server::AsioTcpConnection::~AsioTcpConnection()
{
    // _socket.close();
}

void es::server::AsioTcpConnection::readMessage()
{
    if (!_socket.is_open()) {
        std::cout << "socket is closed" << std::endl;
        return;
    }
    // char buffer[MSGMAX];
    std::cout << "ok" << std::endl;

    // std::memset(_buffer, 0, MSGMAX);
    // boost::asio::rea
    _socket.async_read_some(boost::asio::buffer(_buffer, MSGMAX), [this](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            std::cout << "[SERVER EASYSTREAM]: message received: " << _buffer << std::endl;
            std::string msg(_buffer);
            json tmp;
            // tmp.parse(msg);
            _messages.push_back(msg);
        } else
            std::cout << "[SERVER EASYSTREAM]: ERROR READING MESSAGE: " << ec.message() << std::endl;
        writeMessage("yes");
        readMessage();
    });
    // boost::asio::async_read(_socket, boost::asio::buffer(_buffer, 2048), [this](boost::system::error_code ec, std::size_t length) {
    //     if (!ec) {
    //         std::cout << "[SERVER EASYSTREAM]: message received: " << _buffer << std::endl;
    //         std::string msg(_buffer);
    //         _messages.push_back(msg);
    //     } else
    //         std::cout << "[SERVER EASYSTREAM]: ERROR READING MESSAGE: " << ec.message() << std::endl;
    //     readMessage();
    // });
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
            _socket.close();
        }
    });
}

const boost::asio::ip::tcp::socket &es::server::AsioTcpConnection::getSocket() const
{
    return (_socket);
}