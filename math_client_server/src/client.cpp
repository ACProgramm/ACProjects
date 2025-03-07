#include "client.h"

void connect_to_server(const std::string& host, int port) {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        boost::asio::connect(socket, endpoints);
        std::cout << "Подключение успешно!" << std::endl;


            std::string input;
            std::cout << "Введите выражение ";
            std::getline(std::cin, input);

            boost::asio::write(socket, boost::asio::buffer(input));

            std::array<char, 1024> response;
            boost::system::error_code error;
            size_t length = socket.read_some(boost::asio::buffer(response), error);

            if (error) {
                std::cout << "Ошибка получения ответа!" << std::endl;
                break;
            }

            std::cout << "Результат: " << std::string(response.data(), length) << std::endl;


    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}
