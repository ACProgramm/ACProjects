#include "server.h"

int main() {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

    std::cout << "Сервер запущен. Ожидание клиентов...\n";

    while (true) {
        auto socket = std::make_shared<tcp::socket>(io_context);
        acceptor.accept(*socket);
        std::thread(handle_client, socket).detach(); 
    }

    return 0;
}
