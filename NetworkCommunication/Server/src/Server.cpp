/*
** EPITECH PROJECT, 2021
** easystream_main_plugin
** File description:
** Server
*/

#include <nlohmann/json.hpp>
#include "Server.hpp"

using nlohmann::json;

es::Server::Server(int port, int mq): _maxQueue(mq), _port(port)
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    _serverAdress.sin_family = AF_INET;
    _serverAdress.sin_port = htons(port);
    _serverAdress.sin_addr.s_addr = htonl(INADDR_ANY);
}

es::Server::~Server()
{
    close(_socket);
}

void es::Server::connect()
{
    if (bind(_socket, (struct sockaddr *)&_serverAdress, sizeof(_serverAdress)) == -1) {
        perror("Bind: ");
        throw es::Error("Binding failed", "Server.cpp, line 25");
    }
    std::cout << "Succesfully binded" << std::endl;

    if (listen(_socket, _maxQueue) == -1) {
        perror("Listen: ");
        throw es::Error("Listen failed", "Server.cpp, line 31");
    }
    std::cout << "Succesfully listened" << std::endl;

}

void es::Server::loop()
{
    while (1) {
        FD_ZERO(&_readFds);
        FD_SET(0, &_readFds);
        FD_SET(_socket, &_readFds);
        for (const auto &fd: _clientSockets)
            FD_SET(fd, &_readFds);
        std::cout << "Okay" << std::endl;
        if (select(FD_SETSIZE, &_readFds, NULL, NULL, NULL) == -1) {
            perror("Select: ");
            throw es::Error("Select failed", "Server.cpp, line 44");
        }
        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (!FD_ISSET(i, &_readFds))
                continue;
            if (i == _socket)
                try {
                    acceptClient();
                } catch (const es::Error &e) {
                   std::cout << e.what() << " " << e.where() << std::endl;
                } catch (const std::exception &e) {
                   std::cout << e.what()  << std::endl;
                }
            else {
                try {
                    receiveMessage(i);
                } catch (const es::Error &e) {
                   std::cout << e.what() << " " << e.where() << std::endl;
                } catch (const std::exception &e) {
                   std::cout << e.what()  << std::endl;
                }
            }
        }
    }
}

void es::Server::acceptClient()
{
    json tmpJson;

    socklen_t tmpAddr = sizeof(_serverAdress);
    int newSocket = accept(_socket, (struct sockaddr *)(&_serverAdress), &tmpAddr);

    if (newSocket < 0) {
        perror("Accept: ");
        throw es::Error("Accecpt failed", "Server.cpp, line 56");
    }
    FD_SET(newSocket, &_readFds);
    _clientSockets.push_back(newSocket);
    tmpJson["socketNumber"] = newSocket;
    tmpJson["Message"] = std::string("succesfully connected");
    std::cout << "[SERVER] new connection detected: " << newSocket << std::endl;
    sendMessage(newSocket, tmpJson.dump());
}

void es::Server::sendMessage(int sock, const std::string &msg)
{
    if (send(sock, msg.c_str(), msg.length(), 0)) {
        perror("Send: ");
        throw es::Error("Send failed", "Server.cpp, line 67");
    }
    std::cout << "[SERVER] message sent to client " << sock << std::endl;
}

void es::Server::receiveMessage(int sock)
{
    char buffer[MSGMAX];

    std::memset(buffer, 0, MSGMAX);
    if (recv(sock, buffer, MSGMAX, 0) == -1) {
        perror("Recv: ");
        throw es::Error("Recv failed", "Server.cpp, line 88");
    }
    std::cout << "[SERVER] message received from client: " << sock << std::endl;
    std::cout << "[" << buffer << "]" << std::endl;
}

int main(void)
{
    es::Server server(8080, 30);

    try {
        server.connect();
        server.loop();
    } catch (const es::Error &e) {
        std::cout << e.what() << " " << e.where() << std::endl;
    } catch (const std::exception &e) {
        std::cout << e.what()  << std::endl;
    }
    return (0);
}