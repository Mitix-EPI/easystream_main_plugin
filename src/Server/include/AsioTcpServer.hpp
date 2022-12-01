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
#include <unordered_map>
#include <algorithm>
#include "AsioTcpConnection.hpp"
// #include "../../../../include/json.hpp"

namespace es::server
{
    enum TriggerType
    {
        APP_CHANGE = 0,
        KEY_WORD = 1
    };

    typedef struct
    {
        // Trigger type.
        TriggerType type;
        // Trigger value (ex: is type is TriggerType::KEY_WORD, value is the trigger word).
        std::string value;
        // Scene to change to when triggered.
        std::string target_scene_name;
    } st_instruction_t;

    class AsioTcpServer : public boost::enable_shared_from_this<AsioTcpServer>
    {
    public:
        /***********/
        /* Methods */
        /***********/

        AsioTcpServer(const std::string &host, int port, const std::unordered_map<std::string, std::shared_ptr<obs::AutoAudioLeveler>> &);
        ~AsioTcpServer();

        // --- NETWORK
        bool start();
        void writeMessage(const std::string &);
        void update();

        // --- GETTER
        boost::asio::io_context &getContext();

    private:
        // --- NETWORK
        void waitForClientConnection();

        // --- GET REQUESTS
        void getAllMics(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);
        void getAllEvents(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);

        // --- SET REQUESTS
        void setAutoAudioLeveler(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);
        void setMicLevel(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);
        void setSceneSwapTrigger(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);

        // --- BAD REQUESTS
        void badCommand(boost::shared_ptr<AsioTcpConnection> &);

        /********************/
        /* Member variables */
        /********************/

        // --- Thread
        boost::thread _threadContext;
        // --- Network
        boost::asio::ip::tcp::acceptor _acceptor;
        boost::asio::ip::tcp::endpoint _endPoint;
        boost::asio::io_context _ioContext;
        std::vector<boost::shared_ptr<AsioTcpConnection>> _connections;
        // --- Request handler vars
        std::unordered_map<std::string, void (AsioTcpServer::*)(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &)> _handler;
        std::vector<st_instruction_t> _sceneSwapTriggers;
        const std::unordered_map<std::string, std::shared_ptr<obs::AutoAudioLeveler>> &_audioLeveler;
    };
}

#endif /* !ASIOTCPSERVER_HPP_ */
