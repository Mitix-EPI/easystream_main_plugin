/*
** EPITECH PROJECT, 2021
** NetworkCommunication
** File description:
** AsioTcpServer
*/

#include "../include/AsioTcpServer.hpp"

using nlohmann::json;

es::server::AsioTcpServer::AsioTcpServer(const std::string &host, int port, const std::unordered_map<std::string, std::shared_ptr<obs::AutoAudioLeveler>> &_mps) : _audioLeveler(_mps), _endPoint(boost::asio::ip::make_address(host), port), _acceptor(_ioContext, _endPoint)
{
    _handler["getAllMics"] = &AsioTcpServer::getAllMics;
    _handler["setVolumeToMic"] = &AsioTcpServer::setVolumeToMic;
    _handler["setSceneSwapTrigger"] = &AsioTcpServer::setSceneSwapTrigger;
}

es::server::AsioTcpServer::~AsioTcpServer()
{
}

bool es::server::AsioTcpServer::start()
{
    try
    {
        waitForClientConnection();
        _threadContext = boost::thread([this]()
                                       { _ioContext.run(); });
        blog(LOG_INFO, "[SERVER EASYSTREAM] new server started on port: %d", _endPoint.port());
    }
    catch (std::exception &e)
    {
        blog(LOG_INFO, "[SERVER EASYSTREAM] Exception: %s", e.what());
        return false;
    }

    return true;
}

void es::server::AsioTcpServer::waitForClientConnection()
{
    _acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket sock)
                           {
        if (!ec) {
            std::cout << "[SERVER EASYSTREAM] new connection detected " << sock.remote_endpoint() << std::endl;
            _connections.push_back(boost::make_shared<AsioTcpConnection>(sock));
            json tmpJson;

            _connections.back()->readMessage();
            tmpJson["socketAdress"] = _connections.back()->getSocket().remote_endpoint().address().to_string();
            tmpJson["socketPort"] = _connections.back()->getSocket().remote_endpoint().port();
            tmpJson["Message"] = std::string("succesfully connected");
            tmpJson["statusCode"] = 200; 
            _connections.back()->writeMessage(tmpJson.dump());
        } else {
            std::cout << "[SERVER EASYSTREAM] connection denied: " << ec.message() << std::endl;
        }
        waitForClientConnection(); });
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
    _connections.erase(std::remove_if(_connections.begin(), _connections.end(), [this](const boost::shared_ptr<AsioTcpConnection> con)
                                      { return (!con->isConnected()); }),
                       _connections.end());
    for (auto &con : _connections)
    {
        std::vector<nlohmann::json> _requests = con->getMessage();
        if (_requests.empty())
            continue;
        for (const auto &req : _requests)
        {
            if (_handler.find(req["command"]) != _handler.end())
                (this->*_handler[req["command"]])(req, con);
            else
                badCommand(con);
        }
    }
}

void es::server::AsioTcpServer::getAllMics(const nlohmann::json &j, boost::shared_ptr<AsioTcpConnection> &con)
{
    std::vector<json> mics = es::utils::obs::listHelper::GetMicsList();
    json toSend;

    for (auto &m : mics)
    {
        float tmpValue = _audioLeveler.find(m["micName"])->second->getDesiredLevel() + 60;
        m["value"] = floor((tmpValue * 100) / 60);
        m["isActive"] = _audioLeveler.find(m["micName"])->second->isActive();
    }
    toSend["length"] = mics.size();
    toSend["mics"] = mics;
    toSend["statusCode"] = 200;
    toSend["message"] = std::string("OK");
    con->writeMessage(toSend.dump());
}

void es::server::AsioTcpServer::setVolumeToMic(const nlohmann::json &j, boost::shared_ptr<AsioTcpConnection> &con)
{
    json toSend;
    float value = j["args"]["value"];

    auto tmp = _audioLeveler.find(j["args"]["micId"]);
    if (tmp == _audioLeveler.end())
    {
        // std::string tmpJ = j["micId"];
        toSend["statusCode"] = 404;
        toSend["message"] = " not found";
    }
    else
    {
        float level = (value * 60) / 100;
        level -= 60;
        std::cout << "level is: " << level << std::endl;
        tmp->second->setDesiredLevel(level);
        toSend["statusCode"] = 200;
        toSend["message"] = std::string("OK");
    }
    con->writeMessage(toSend.dump());
}

void es::server::AsioTcpServer::badCommand(boost::shared_ptr<AsioTcpConnection> &con)
{
    nlohmann::json toSend;
    toSend["statusCode"] = 404;
    toSend["message"] = "The requested action does not exist";
    con->writeMessage(toSend.dump());
}

void es::server::AsioTcpServer::setSceneSwapTrigger(const nlohmann::json &j, boost::shared_ptr<AsioTcpConnection> &con)
{
    json toSend;
    int j_trigger_type = j["args"]["triggerType"];
    auto j_trigger_value = std::string(j["args"]["triggerValue"]);
    auto j_target_scene_name = std::string(j["args"]["targetScene"]);

    // Find target scene
    bool target_scene_not_found = false; // @dev (Romain) : Hidden stuff
    for (const auto scene : es::utils::obs::listHelper::GetSceneList())
    {
        if (j_target_scene_name == std::string(scene["sceneName"]))
        {
            target_scene_not_found = true;
            break;
        }
    }

    // Check if type in TriggerType enum values
    if (j["args"]["triggerType"] < 0 || j["args"]["triggerType"] > 1)
    {
        toSend["statusCode"] = 404;
        toSend["message"] = std::string("Trigger type not found");
    }
    else if (target_scene_not_found)
    {
        toSend["statusCode"] = 404;
        toSend["message"] = std::string("Target scene not found");
    }
    else // Request is valid
    {
        auto triggerInstruction = st_instruction_t({TriggerType(j_trigger_type),
                                                    j_trigger_value,
                                                    j_target_scene_name});
        this->_sceneSwapTriggers.push_back(triggerInstruction);

        blog(
            LOG_INFO,
            "[SERVER EASYSTREAM] added instruction :\n--- type : %d\n--- value : %s\n--- target_scene_name : %s",
            triggerInstruction.type,
            triggerInstruction.value.c_str(),
            triggerInstruction.target_scene_name.c_str());

        toSend["statusCode"] = 200;
        toSend["message"] = std::string("OK");
    }
    con->writeMessage(toSend.dump());
}

// 60 -> 100
// 0

//-40 + 60
//
