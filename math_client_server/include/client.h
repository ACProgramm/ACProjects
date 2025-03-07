#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

void connect_to_server(const std::string& host, int port);
