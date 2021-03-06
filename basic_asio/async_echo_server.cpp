// Copyright (c) 2012-2013 Bryce Adelstein-Lelbach
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/asio.hpp>

using boost::system::error_code;
namespace asio = boost::asio;
typedef boost::asio::ip::tcp asio_tcp;

int main()
{
    try
    {
        asio::io_service io_service;

        asio_tcp::endpoint endpoint(asio_tcp::v4(), 2000);
        asio_tcp::acceptor acceptor(io_service, endpoint);

        for (;;)
        {
            asio_tcp::socket socket(io_service);

            error_code ec;
            acceptor.accept(socket, ec);
            if (ec) continue;

            std::size_t const max_length = 1024;
            char msg[max_length];

            std::function<void(error_code const&, std::size_t)>
                f = [&](error_code const& ec, std::size_t bytes)
                    {
                        if (ec) return;
                        std::cout << std::string(msg, bytes) << "\n";
                        asio::async_write(socket, asio::buffer(msg, bytes),
                            [&](error_code const& ec, std::size_t)
                            {
                                if (ec) return;
                                auto buf = asio::buffer(msg, max_length);
                                socket.async_read_some(buf, f);
                            });
                    };

            socket.async_read_some(asio::buffer(msg, max_length), f);

            io_service.run();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

