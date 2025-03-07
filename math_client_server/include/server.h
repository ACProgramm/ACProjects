#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>

using boost::asio::ip::tcp;

void handle_client(std::shared_ptr<tcp::socket> socket);
double calculate_expression(const std::string& expression);
