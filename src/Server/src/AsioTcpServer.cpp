/*
** EPITECH PROJECT, 2021
** NetworkCommunication
** File description:
** AsioTcpServer
*/

#include "../include/AsioTcpServer.hpp"

namespace es::server
{
    AsioTcpServer::AsioTcpServer(const std::string &host, int port, const std::unordered_map<std::string, std::shared_ptr<obs::AutoAudioLeveler>> &_mps) : _audioLeveler(_mps), _endPoint(boost::asio::ip::make_address(host), port), _acceptor(_ioContext, _endPoint)
    {
        /* Getters */
        _handler["getAllMics"] = &AsioTcpServer::getAllMics;
        // _handler["getActReactCouples"]

        /* Setters */
        _handler["setAutoAudioLeveler"] = &AsioTcpServer::setAutoAudioLeveler;
        _handler["setMicLevel"] = &AsioTcpServer::setMicLevel;
        // _handler["setTranscriptSubscription"]
        // _handler["setActionReaction"]
        _handler["setSceneSwapTrigger"] = &AsioTcpServer::setSceneSwapTrigger;

        /* Removers */
        // _handler["removeActReact"]

        /* Updaters */
        // _handler["updateAction"];
        // _handler["updateReaction"];
    }

    AsioTcpServer::~AsioTcpServer()
    {
    }

    /***********/
    /* NETWORK */
    /***********/

    bool AsioTcpServer::start()
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

    void AsioTcpServer::waitForClientConnection()
    {
        this->_acceptor.async_accept(
            // Callback on new connection acceptance
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket sock)
            {
                if (!ec) // true == New connection success
                {
                    json tmpJson;

                    std::cout << "[SERVER EASYSTREAM] new connection detected " << sock.remote_endpoint() << std::endl;
                    _connections.push_back(CreateShared<AsioTcpConnection>(sock));

                    _connections.back()->readMessage();
                    tmpJson["socketAdress"] = _connections.back()->getSocket().remote_endpoint().address().to_string();
                    tmpJson["socketPort"] = _connections.back()->getSocket().remote_endpoint().port();
                    tmpJson["Message"] = std::string("succesfully connected");
                    tmpJson["statusCode"] = 200;
                    _connections.back()->writeMessage(tmpJson.dump());
                }
                else // New connection error
                {
                    std::cout << "[SERVER EASYSTREAM] connection denied: " << ec.message() << std::endl;
                }
                waitForClientConnection();
            });
    }

    void AsioTcpServer::writeMessage(const std::string &msg)
    {
        if (_connections.size() > 0)
        {
            _connections[0]->writeMessage(msg);
        }
    }

    void AsioTcpServer::update()
    {
        // Remove disconnected sockets
        this->_connections.erase(
            std::remove_if(
                this->_connections.begin(),
                this->_connections.end(),
                [this](const Shared<AsioTcpConnection> con)
                {
                    return (!con->isConnected());
                }),
            this->_connections.end());

        // Treat requests
        for (auto &con : this->_connections)
        {
            std::vector<json> requests_ = con->getMessage();

            if (requests_.empty()) // @dev (Romain) : Is necessary ?
            {
                continue;
            }
            for (const auto &req : requests_)
            {
                if (_handler.find(req["command"]) != _handler.end())
                {
                    (this->*_handler[req["command"]])(req, con);
                }
                else
                {
                    this->badCommand(con);
                }
            }
        }
    }

    boost::asio::io_context &AsioTcpServer::getContext()
    {
        return this->_ioContext;
    }

    /****************/
    /* GET REQUESTS */
    /****************/

    void AsioTcpServer::getAllMics(const json &j, Shared<AsioTcpConnection> &con)
    {
        json toSend;
        std::vector<json> mics = es::utils::obs::listHelper::GetMicsList();

        for (auto &m : mics)
        {
            float tmpValue = _audioLeveler.find(m["micName"])->second->getDesiredLevel() + 60;

            m["value"] = floor((tmpValue * 100) / 60);
            m["isActive"] = _audioLeveler.find(m["micName"])->second->isActive();
        }

        toSend["statusCode"] = 200;
        toSend["message"] = std::string("OK");
        toSend["length"] = mics.size();
        toSend["mics"] = mics;

        con->writeMessage(toSend.dump());
    }

    /****************/
    /* SET REQUESTS */
    /****************/

    void AsioTcpServer::setAutoAudioLeveler(const json &j, Shared<AsioTcpConnection> &con)
    {
        json toSend;
        json args_ = j["args"];
        bool enable_ = args_["enable"];
        auto source_ = this->_audioLeveler.find(args_["source"]);

        if (source_ == this->_audioLeveler.end())
        {
            toSend["statusCode"] = 404;
            toSend["message"] = std::string("Not found");
        }
        else if (enable_)
        {
            // enable
        }
        else
        {
            // disable
        }
        con->writeMessage(toSend.dump());
    }

    void AsioTcpServer::setMicLevel(const json &j, Shared<AsioTcpConnection> &con)
    {
        json toSend;
        float value = j["args"]["value"];
        auto tmp = _audioLeveler.find(j["args"]["micId"]);

        if (tmp == _audioLeveler.end())
        {
            toSend["statusCode"] = 404;
            toSend["message"] = std::string("Not found");
        }
        else
        {
            float level = ((value * 60) / 100) - 60;
            // level -= 60;
            // std::cout << "level is: " << level << std::endl;
            tmp->second->setDesiredLevel(level);
            toSend["statusCode"] = 200;
            toSend["message"] = std::string("OK");
        }
        con->writeMessage(toSend.dump());
    }

    void AsioTcpServer::setSceneSwapTrigger(const json &j, Shared<AsioTcpConnection> &con)
    {
        json toSend;
        int j_trigger_type = j["args"]["triggerType"];
        auto j_trigger_value = std::string(j["args"]["triggerValue"]);
        auto j_target_scene_name = std::string(j["args"]["targetScene"]);

        // Find target scene
        // @dev (Romain) : use std::find_if instead
        bool target_scene_not_found = false;
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

    /****************/
    /* BAD REQUESTS */
    /****************/

    void AsioTcpServer::badCommand(Shared<AsioTcpConnection> &con)
    {
        json toSend;

        toSend["statusCode"] = 404;
        toSend["message"] = "The requested action does not exist";
        con->writeMessage(toSend.dump());
    }
}

// 60 -> 100
// 0

//-40 + 60
//
