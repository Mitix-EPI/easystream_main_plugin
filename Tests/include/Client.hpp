#ifndef CLIENT_HPP
#define CLIENT_HPP
#define MSGMAX 4096
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "../../src/nlohmann/json.hpp"

namespace es
{
    namespace test
    {
        class Client
        {
            public:
                Client();
                ~Client();

                const std::string &getLastResponse();
                void sendMessage(const std::string &message);
                // void update();
                nlohmann::json readMessage();
                void connect();
                const boost::asio::ip::tcp::socket &getSocket();

            private:
                boost::asio::io_context _io_context;
                boost::asio::ip::tcp::socket _sock;
                std::string response;
                char _buffer[MSGMAX];
                bool _connected;
                boost::asio::ip::tcp::endpoint _endpoint;
        };
    }
}

#endif