/*
** EPITECH PROJECT, 2021
** NetworkCommunication
** File description:
** AsioTcpConnection
*/

#ifndef ASIOTCPCONNECTION_HPP_
#define ASIOTCPCONNECTION_HPP_
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include "../../nlohmann/json.hpp"
#define MSGMAX 2048

namespace es {
    namespace server {
        class AsioTcpConnection: public boost::enable_shared_from_this<AsioTcpConnection> {
            public:
                AsioTcpConnection(boost::asio::ip::tcp::socket &sock);
                ~AsioTcpConnection();

                void readMessage();
                void writeMessage(const std::string &msg);
                const boost::asio::ip::tcp::socket &getSocket() const;
            private:
                char _buffer[MSGMAX];
                boost::asio::ip::tcp::socket _socket;
                std::vector<std::string> _messages;
        };
    }
}


#endif /* !ASIOTCPCONNECTION_HPP_ */
