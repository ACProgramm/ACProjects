#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using boost::asio::ip::tcp;

class ChatClient {
public:
    ChatClient(boost::asio::io_context& io_context, const std::string& host, short port)
        : socket_(io_context) {
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        boost::asio::async_connect(socket_, endpoints,
            [this](boost::system::error_code error, tcp::endpoint) {
                if (!error) {
                    std::cout << "Подключено к серверу!\n";
                    startRead();
                    startInputThread();
                }
            });
    }

private:
    tcp::socket socket_;

    void startRead() {
        auto buffer = std::make_shared<std::array<char, 1024>>();

        socket_.async_read_some(boost::asio::buffer(*buffer),
            [this, buffer](boost::system::error_code error, std::size_t length) {
                if (!error) {
                    std::cout << ">> " << std::string(buffer->data(), length) << '\n';
                    startRead();
                }
            });
    }

    void sendMessage(const std::string& message) {
        auto response = std::make_shared<std::string>(message);

        boost::asio::async_write(socket_, boost::asio::buffer(*response),
            [response](boost::system::error_code error, std::size_t) {
                if (error) {
                    std::cout << "Ошибка отправки: " << error.message() << '\n';
                }
            });
    }

    void startInputThread() {
        std::thread([this]() {
            while (true) {
                std::string message;
                std::getline(std::cin, message);
                if (message == "exit") {
                    socket_.close();
                    break;
                }
                sendMessage(message);
            }
            }).detach();
    }
};

int main() {
    setlocale(LC_ALL, "");
    boost::asio::io_context io_context;
    ChatClient client(io_context, "127.0.0.1", 8080);
    io_context.run();
    return 0;
}
