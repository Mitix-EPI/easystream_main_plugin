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

namespace es
{
    namespace server
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
            AsioTcpServer(const std::string &host, int port, const std::unordered_map<std::string, std::shared_ptr<obs::AutoAudioLeveler>> &);
            ~AsioTcpServer();

            bool start();
            boost::asio::io_context &getContext();
            void writeMessage(const std::string &);
            void update();

        private:
            void getAllMics(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);
            void getAllEvents(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);
            void setVolumeToMic(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);
            void setSceneSwapTrigger(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &);
            void badCommand(boost::shared_ptr<AsioTcpConnection> &);
            void waitForClientConnection();

            /* Member variables */
            const std::unordered_map<std::string, std::shared_ptr<obs::AutoAudioLeveler>> &_audioLeveler;
            std::unordered_map<std::string, void (AsioTcpServer::*)(const nlohmann::json &, boost::shared_ptr<AsioTcpConnection> &)> _handler;
            std::vector<boost::shared_ptr<AsioTcpConnection>> _connections;
            boost::thread _threadContext;
            boost::asio::ip::tcp::endpoint _endPoint;
            boost::asio::io_context _ioContext;
            boost::asio::ip::tcp::acceptor _acceptor;
            // --- Scene swap trigger
            std::vector<st_instruction_t> _sceneSwapTriggers;
        };
    }
}

#endif /* !ASIOTCPSERVER_HPP_ */
