/*
** EPITECH PROJECT, 2021
** NetworkCommunication
** File description:
** AsioTcpServer
*/

#include "../include/AsioTcpServer.hpp"

using nlohmann::json;

es::server::AsioTcpServer::AsioTcpServer(const std::string &host, int port): _endPoint(boost::asio::ip::make_address(host), port), _acceptor(_ioContext, _endPoint)
{
    _handler["getAllMics"] = &AsioTcpServer::getAllMics;
}

es::server::AsioTcpServer::~AsioTcpServer()
{
}

bool es::server::AsioTcpServer::start()
{
    try {
        waitForClientConnection();
        _threadContext = boost::thread([this](){_ioContext.run();});
        blog(LOG_INFO, "[SERVER EASYSTREAM] new server started on port: %d", _endPoint.port());
    } catch (std::exception &e) {
        blog(LOG_INFO, "[SERVER EASYSTREAM] Exception: %s", e.what());
        return false;
    }
    return true;
}

void es::server::AsioTcpServer::waitForClientConnection()
{
    _acceptor.async_accept([this] (boost::system::error_code ec, boost::asio::ip::tcp::socket sock) {
        if (!ec) {
            std::cout << "[SERVER EASYSTREAM] new connection detected " << sock.remote_endpoint() << std::endl;
            _connections.push_back(boost::make_shared<AsioTcpConnection>(sock));
            json tmpJson;

            _connections.back()->readMessage();
            tmpJson["socketNumber"] = _connections.back()->getSocket().remote_endpoint().address().to_string();
            tmpJson["Message"] = std::string("succesfully connected");
            _connections.back()->writeMessage(tmpJson.dump());
        } else {
            std::cout << "[SERVER EASYSTREAM] connection denied: " << ec.message() << std::endl;
        }
        waitForClientConnection();
    });
}

boost::asio::io_context &es::server::AsioTcpServer::getContext()
{
    return (_ioContext);
}

void es::server::AsioTcpServer::writeMessage(const std::string &msg)
{
    if (_connections.size() > 0)
        _connections[0]->writeMessage(msg);
}

void es::server::AsioTcpServer::update()
{
    _connections.erase(std::remove_if(_connections.begin(), _connections.end(), [this](const boost::shared_ptr<AsioTcpConnection> &con){
        return (!con->isConnected());
    }), _connections.end());
    for (auto &con: _connections) {
        std::vector<nlohmann::json> _requests = con->getMessage();
        if (_requests.empty())
            continue;
        for (const auto &req: _requests)
            (this->*_handler[req["command"]])(req, con); 
    }
}

void es::server::AsioTcpServer::getAllMics(const nlohmann::json &j, boost::shared_ptr<AsioTcpConnection> &con)
{
    std::vector<json> mics = es::utils::obs::listHelper::GetMicsList();
    // std::vector<std::string> micStr;
    json toSend;

    // for (const auto &j: mics)
    //     micStr.push_back(j.dump());
    // json tmp = json::parse(mics.begin(), mics.end());

    toSend["length"] = mics.size();
    toSend["mics"] = mics;
    con->writeMessage(toSend.dump());
}