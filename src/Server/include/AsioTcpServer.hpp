/*
** EPITECH PROJECT, 2021
** NetworkCommunication
** File description:
** AsioTcpServer
*/

#ifndef ASIOTCPSERVER_HPP_
#define ASIOTCPSERVER_HPP_
#include "IServer.hpp"
#include <thread>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/make_shared.hpp>
// #include <boost/thread.hpp>
#include <boost/thread.hpp>
#include "AsioTcpConnection.hpp"
// #include "../../../../include/json.hpp"

namespace es {
    namespace server{
        class AsioTcpServer : public IServer, public boost::enable_shared_from_this<AsioTcpServer>
        {
            public:
                AsioTcpServer(const std::string &host, int port);
                ~AsioTcpServer();

                bool start() final;
                // void stop() final;
                // void run() final;
                // void sendMessage(const std::string &) final;
                // const std::string &receiveMessage() final;
                boost::asio::io_context &getContext() final;
                void writeMessage(const std::string &);
            private:
                void waitForClientConnection();
                std::vector<boost::shared_ptr<AsioTcpConnection>> _connections;
                boost::thread _threadContext;
                boost::asio::ip::tcp::endpoint _endPoint;
                boost::asio::io_context _ioContext;
                boost::asio::ip::tcp::acceptor _acceptor;
        };
    }
}


#endif /* !ASIOTCPSERVER_HPP_ */
