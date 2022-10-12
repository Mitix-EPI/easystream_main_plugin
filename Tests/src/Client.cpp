#include <Client.hpp>


es::test::Client::Client(): _sock(_io_context), _connected(false), _endpoint(boost::asio::ip::make_address("0.0.0.0"), 47920)
{
    std::memset(_buffer, 0, MSGMAX);
    // _sock.connect(_endpoint);
}

es::test::Client::~Client()
{

}

void es::test::Client::connect()
{
    _sock.connect(_endpoint);
}

void es::test::Client::sendMessage(const std::string &message)
{
    // nlohmann::json toSend;
    char buffer[MSGMAX];
    // std::string tmp;

    // toSend["command"] = message;
    // tmp = toSend.dump();
    std::memset(buffer, 0, MSGMAX);
    std::memcpy(buffer, message.data(), message.size());
    _sock.write_some(boost::asio::buffer(buffer, message.size()));
}

const boost::asio::ip::tcp::socket &es::test::Client::getSocket()
{
    return (_sock);
}

nlohmann::json es::test::Client::readMessage()
{
    std::string message;
    nlohmann::json tmp;
    boost::system::error_code error;
    size_t len = _sock.read_some(boost::asio::buffer(_buffer));
    
    if (error = boost::asio::error::eof)
        _connected = false;
    else if (error)
        throw boost::system::system_error(error);
    if (!len)
        return (tmp);
    tmp = nlohmann::json::parse(_buffer);
    std::cout << _buffer << std::endl;
    std::memset(_buffer, 0, MSGMAX);
    return (tmp);
}