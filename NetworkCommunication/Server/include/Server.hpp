/*
** EPITECH PROJECT, 2021
** easystream_main_plugin
** File description:
** Server
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include "Error.hpp"
#define MSGMAX 2048

namespace es {
    class Server {
        public:
            Server(int port, int mq);
            ~Server();

            void connect();
            void loop();
            void receiveMessage(int sock);
            void sendMessage(int sock, const std::string &msg);
            void acceptClient();
        private:
            fd_set _readFds;
            std::vector<int> _clientSockets;
            // int _fd;
            struct sockaddr_in _serverAdress;
            int _socket;
            int _maxQueue;
            int _port;
    };
}


#endif /* !SERVER_HPP_ */
