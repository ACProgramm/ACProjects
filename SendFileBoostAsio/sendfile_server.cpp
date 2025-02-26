#include <boost/asio.hpp>
#include <iostream>
#include <unordered_set>
#include <memory>
#include <fstream>

using boost::asio::ip::tcp;

void sendFile(tcp::socket& socket) {
    std::ifstream file("D://Games//test.txt", std::ios::binary);
    if (!file) {
        std::cerr << "Ошибка: Файл test.txt не найден!\n";
        return;
    }

    std::array<char, 1024> buffer;
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        boost::system::error_code error;
        boost::asio::write(socket, boost::asio::buffer(buffer.data(), file.gcount()), error);
        if (error) {
            std::cerr << "Ошибка отправки файла: " << error.message() << '\n';
            return;
        }
    }

    std::cout << "Файл успешно отправлен!\n";
}

int main() {
    setlocale(LC_ALL, "Ru");

    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

    std::cout << "Сервер запущен. Ожидание клиентов...\n";

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        std::cout << "Клиент подключился, отправляем файл...\n";

        sendFile(socket);
    }

    return 0;
}
