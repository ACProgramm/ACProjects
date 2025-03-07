#include "server.h"

void handle_client(std::shared_ptr<tcp::socket> socket) {
    try {
        std::array<char, 1024> buffer;
        boost::system::error_code error;

        while (true) {

            size_t length = socket->read_some(boost::asio::buffer(buffer), error);
            if (error) {
                std::cout << "Ошибка чтения: " << error.message() << '\n';
                break;
            }

            std::string request(buffer.data(), length);
            std::cout << "Запрос от клиента: " << request << '\n';


            double result = calculate_expression(request);


            std::string response = std::to_string(result);
            boost::asio::write(*socket, boost::asio::buffer(response));
            std::cout << "Ответ отправлен клиенту: " << response << '\n';
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка обработки клиента: " << e.what() << '\n';
    }
}

double calculate_expression(const std::string& expression) {
    std::istringstream iss(expression);

    double num1, num2;
    char op;

    if (!(iss >> num1 >> op >> num2)) {
        throw std::runtime_error("Ошибка: некорректное выражение!");
    }

    switch (op) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/':
            if (num2 == 0) throw std::runtime_error("Ошибка: деление на ноль!");
            return num1 / num2;
        default:
            throw std::runtime_error("Ошибка: неизвестный оператор!");
    }
}
