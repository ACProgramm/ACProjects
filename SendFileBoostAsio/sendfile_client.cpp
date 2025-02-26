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
        std::cout << "����������� � ������� �������!\n";
    }
    catch (std::exception& e) {
        std::cerr << "������ �����������: " << e.what() << std::endl;
    }
}

void receiveFile(tcp::socket& socket, const std::string& filename) {
    try {
        std::string save_path = "D://Games//" + filename;
        std::ofstream output(save_path, std::ios::binary);
        if (!output) {
            std::cerr << "������ �������� �����!\n";
            return;
        }

        std::array<char, 1024> buffer;
        boost::system::error_code error;

        while (true) {
            size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer), error);
            if (error == boost::asio::error::eof) break;  // ������ ������ ���������� (���� �������)
            if (error) {
                std::cerr << "������ ��� ��������� �����: " << error.message() << '\n';
                return;
            }

            output.write(buffer.data(), bytes_transferred);
            std::cout << "�������� " << bytes_transferred << " ����\n";
        }

        std::cout << "���� ������� ��������!\n";
    }
    catch (std::exception& e) {
        std::cerr << "������: " << e.what() << '\n';
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
