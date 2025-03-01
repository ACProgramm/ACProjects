#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <memory>
#include <sstream>
#include <fstream>

using boost::asio::ip::tcp;

std::string handle_request(const std::string& request);
std::string serve_file(const std::string& filename);
std::string get_content_type(const std::string& filename);

void handle_client(std::shared_ptr<tcp::socket> socket) {
    try {
        std::array<char, 1024> buffer;
        boost::system::error_code error;

        // Читаем HTTP-запрос от клиента
        size_t length = socket->read_some(boost::asio::buffer(buffer), error);
        if (error) {
            std::cerr << "Ошибка чтения: " << error.message() << '\n';
            return;
        }

        std::string request(buffer.data(), length);
        std::string response = handle_request(request);

        // Отправляем HTTP-ответ
        boost::asio::write(*socket, boost::asio::buffer(response));
        std::cout << "Ответ отправлен клиенту!\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка обработки клиента: " << e.what() << '\n';
    }
}

std::string handle_request(const std::string& request) {
    std::istringstream request_stream(request);
    std::string method, path, version;
    request_stream >> method >> path >> version;

    std::cout << "Запрос: " << method << " " << path << " " << version << '\n';

    if (path == "/") {
        return "HTTP/1.1 200 OK\r\n"
            "Content-Length: 13\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Главная страница\n";
    }
    else if (path == "/about") {
        return "HTTP/1.1 200 OK\r\n"
            "Content-Length: 17\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "О нас: Boost Server\n";
    }
    else if (path.rfind("/files/", 0) == 0) {  // Проверяем, начинается ли путь с "/files/"
        std::string filename = path.substr(7);  // Убираем "/files/"
        return serve_file(filename);
    }
    else {
        return "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 9\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Не найдено\n";
    }
}

std::string serve_file(const std::string& filename) {
    std::string full_path = "D:/Games/files/" + filename;
    std::ifstream file(full_path, std::ios::binary);
    if (!file) {
        return "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 9\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Не найдено\n";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Читаем файл в строковый поток
    std::string content = buffer.str();

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
        << "Content-Length: " << content.size() << "\r\n"
        << "Content-Type: " << get_content_type(filename) << "\r\n"
        << "\r\n"
        << content;

    return response.str();
}

std::string get_content_type(const std::string& filename) {
    if (filename.ends_with(".html")) return "text/html";
    if (filename.ends_with(".css")) return "text/css";
    if (filename.ends_with(".png")) return "image/png";
    if (filename.ends_with(".jpg") || filename.ends_with(".jpeg")) return "image/jpeg";
    return "application/octet-stream";
}

int main() {
    setlocale(LC_ALL, "Ru");
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

    std::cout << "Сервер запущен. Ожидание клиентов...\n";

    while (true) {
        auto socket = std::make_shared<tcp::socket>(io_context);
        acceptor.accept(*socket);

        // Запускаем клиента в отдельном потоке
        std::thread(handle_client, socket).detach();
    }

    return 0;
}
