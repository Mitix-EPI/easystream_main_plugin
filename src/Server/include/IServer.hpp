/*
** EPITECH PROJECT, 2021
** NetworkCommunication
** File description:
** IServer
*/

#ifndef ISERVER_HPP_
#define ISERVER_HPP_

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../../nlohmann/json.hpp"
#include "../../plugin-main.hpp"
// #include "Error.hpp"

namespace es::server
{
    class IServer
    {
    public:
        virtual ~IServer() = default;
        virtual bool start() = 0;
        // virtual void run() = 0;
        // virtual void stop() = 0;
        // virtual void sendMessage(const std::string &) = 0;
        // virtual const std::string &receiveMessage() = 0;
        virtual boost::asio::io_context &getContext() = 0;
    };
}

#endif /* !ISERVER_HPP_ */
