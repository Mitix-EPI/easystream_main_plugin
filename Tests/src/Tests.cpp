#include "Client.hpp"
#include "Tests.hpp"
#include <chrono>
#include <thread>

namespace es
{
    namespace test
    {
        TEST_F(ServerTest, ConnectionTestTimeout)
        {
            _client.connect();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json result = _client.readMessage();
            ASSERT_FALSE(result.empty());
        }
        TEST_F(ServerTest, ConnectionTestValueOfAdress)
        {
            _client.connect();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json result = _client.readMessage();
            ASSERT_EQ(result["socketAdress"], _client.getSocket().local_endpoint().address().to_string());
        }
        TEST_F(ServerTest, ConnectionTestValueOfPort)
        {
            _client.connect();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json result = _client.readMessage();
            ASSERT_EQ(result["socketPort"], _client.getSocket().local_endpoint().port());
        }
        TEST_F(ServerTest, ConnectionTestStatusCode)
        {
            _client.connect();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json result = _client.readMessage();
            ASSERT_EQ(result["statusCode"], 200);
        }
        TEST_F(ServerTest, TestNumberOfMics)
        {
            _client.connect();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json result = _client.readMessage();
            ASSERT_EQ(result["statusCode"], 200);
            nlohmann::json toSend;
            toSend["command"] = "getAllMics";
            _client.sendMessage(toSend.dump());
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            toSend = _client.readMessage();
            ASSERT_EQ(toSend["statusCode"], 200);
            ASSERT_EQ(toSend["length"], 2);
        }
        // TEST_F(ServerTest, TestServerReceivingAndResponse) {
        //     _client.connect();
        //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //     nlohmann::json result = _client.readMessage();
        //     ASSERT_EQ(result["statusCode"], 200);
        // }
        // TEST_F(ServerTest, TestGetallMics) {
        //     _client.connect();
        //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //     nlohmann::json result = _client.readMessage();
        //     ASSERT_EQ(result["statusCode"], 200);
        // }
        // TEST_F(ServerTest, testSetMics) {
        //     _client.connect();
        //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //     nlohmann::json result = _client.readMessage();
        //     ASSERT_EQ(result["statusCode"], 200);
        // }
        TEST_F(ServerTest, TestNoGoodFunction)
        {
            _client.connect();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json result = _client.readMessage();
            ASSERT_EQ(result["statusCode"], 200);
            nlohmann::json toSend;
            toSend["command"] = "bad command";
            _client.sendMessage(toSend.dump());
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            result = _client.readMessage();
            ASSERT_EQ(result["statusCode"], 404);
        }
        TEST_F(ServerTest, TestSetSceneChangeTrigger)
        {
            _client.connect();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json result = _client.readMessage();
            ASSERT_EQ(result["statusCode"], 200);
            nlohmann::json toSend;
            toSend["command"] = "setSceneSwapTrigger";
            toSend["args"]["triggerType"] = 1;
            toSend["args"]["triggerValue"] = "random_word";
            toSend["args"]["targetScene"] = "MyScene";
            _client.sendMessage(toSend.dump());
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            result = _client.readMessage();
            ASSERT_EQ(result["statusCode"], 200);
        }
        TEST_F(ServerTest, TestSetSceneTriggerWrongType)
        {
            _client.connect();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            nlohmann::json result = _client.readMessage();
            ASSERT_EQ(result["statusCode"], 200);
            nlohmann::json toSend;
            toSend["command"] = "setSceneSwapTrigger";
            toSend["args"]["triggerType"] = 2;
            toSend["args"]["triggerValue"] = "random_word";
            toSend["args"]["targetScene"] = "obs";
            _client.sendMessage(toSend.dump());
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            result = _client.readMessage();
            ASSERT_EQ(result["statusCode"], 404);
            ASSERT_EQ(result["message"], "Trigger type not found");
        }
        // TEST_F(ServerTest, TestTooLongMessage) {
        //     char buffer[MSGMAX];
        //     std::memset(buffer, 0, 101);
        //     std::memset(buffer, '0', 100);
        //     _client.connect();
        //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //     nlohmann::json result = _client.readMessage();
        //     ASSERT_EQ(result["statusCode"], 200);
        //     nlohmann::json toSend;
        //     toSend["message"] = buffer;
        //     _client.sendMessage(toSend.dump());
        //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //     result = _client.readMessage();
        //     ASSERT_EQ(result["statusCode"], 404);
        // }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}