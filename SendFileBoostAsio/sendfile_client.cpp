#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <fstream>

using boost::asio::ip::tcp;

void connectToServer(boost::asio::io_context& io_context, tcp::socket& socket) {
    try {
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "8080");
        boost::asio::connect(socket, endpoints);
        std::cout << "Подключение к серверу успешно!\n";
    }
    catch (std::exception& e) {
        std::cerr << "Ошибка подключения: " << e.what() << std::endl;
    }
}

void receiveFile(tcp::socket& socket, const std::string& filename) {
    try {
        std::string save_path = "D://Games//" + filename;
        std::ofstream output(save_path, std::ios::binary);
        if (!output) {
            std::cerr << "Ошибка создания файла!\n";
            return;
        }

        std::array<char, 1024> buffer;
        boost::system::error_code error;

        while (true) {
            size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer), error);
            if (error == boost::asio::error::eof) break;  // Сервер закрыл соединение (файл передан)
            if (error) {
                std::cerr << "Ошибка при получении файла: " << error.message() << '\n';
                return;
            }

            output.write(buffer.data(), bytes_transferred);
            std::cout << "Получено " << bytes_transferred << " байт\n";
        }

        std::cout << "Файл успешно загружен!\n";
    }
    catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);

    connectToServer(io_context, socket);
    receiveFile(socket, "received.txt");

    return 0;
}
