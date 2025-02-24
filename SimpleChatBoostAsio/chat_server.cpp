#include <boost/asio.hpp>
#include <iostream>
#include <unordered_set>
#include <memory>

using boost::asio::ip::tcp;

class ChatServer {
public:
    ChatServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        std::cout << "Сервер запущен. Ожидание клиентов...\n";
        startAccept();
    }

private:
    tcp::acceptor acceptor_;
    std::unordered_set<std::shared_ptr<tcp::socket>> clients_;

    void startAccept() {
        auto socket = std::make_shared<tcp::socket>(acceptor_.get_executor());

        acceptor_.async_accept(*socket, [this, socket](boost::system::error_code error) {
            if (!error) {
                std::cout << "Клиент подключился!\n";
                clients_.insert(socket);
                startRead(socket);
            }
            else {
                std::cout << "Ошибка подключения: " << error.message() << '\n';
            }
            startAccept();
            });
    }


    void startRead(std::shared_ptr<tcp::socket> socket) {
        auto buffer = std::make_shared<std::array<char, 1024>>();

        socket->async_read_some(boost::asio::buffer(*buffer),
            [this, socket, buffer](boost::system::error_code error, std::size_t length) {
                if (!error) {
                    std::string message(buffer->data(), length);

                    std::cout << "Сообщение от клиента: " << message << '\n';
                    broadcastMessage("Клиент: " + message, socket);
                    startRead(socket);

                }
                else {
                    std::cout << "Клиент отключился.\n";
                    clients_.erase(socket);
                }
            });
    }


    void broadcastMessage(const std::string& message, std::shared_ptr<tcp::socket> sender) {
        auto response = std::make_shared<std::string>(message);

        for (auto& client : clients_) {
            if (client != sender) {  // 🔥 Не отправляем сообщение самому себе
                boost::asio::async_write(*client, boost::asio::buffer(*response),
                    [client, response](boost::system::error_code error, std::size_t) {
                        if (error) {
                            std::cout << "Ошибка отправки: " << error.message() << '\n';
                        }
                    });
            }
        }
    }


};



int main() {
    setlocale(LC_ALL, "");
    boost::asio::io_context io_context;
    ChatServer server(io_context, 8080);
    io_context.run();
    return 0;
}
