#pragma once

#include "../common/IWriteDataAccess.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
/**
 * @class TCPClient
 * @brief A class that represents a TCP client for connecting to a server and sending data.
 */
class TCPClient : public IWriteDataAccess
{
public:
    /**
     * @brief Constructs a TCPClient object.
     * 
     * This constructor initializes the TCP client, resolves the server name to a list of endpoints, 
     * and attempts to establish a connection to the server asynchronously.     * 
     * @param serverName The name or IP address of the server to connect to.
     * @param portName The port number or service name to connect to on the server.
     * @throws std::runtime_error if the connection to the server fails.
     */
    TCPClient(std::string_view serverName, std::string_view portName)
	{
		try
		{
			// Resolve the server name into a list of endpoints
			auto endpoints = resolver_.resolve(serverName, portName);
			// Try to connect to one of the resolved endpoints

			boost::asio::async_connect(socket_, endpoints, [this](boost::system::error_code ec, const tcp::endpoint&)
				{
					if (!ec)
					{
						std::cout << "Connected to server\n";
						is_connected_to_server_ = true;
					}
					else
					{
						std::cerr << "Error connecting to server: " << ec.message() << " (Code: " << ec.value() << ")\n";
					}
				});
			try
			{
				io_context_.run(); // Run the io_context to process asynchronous operations
			}
			catch (const std::exception& e)
			{
				std::cerr << "Exception in io_context: " << e.what() << "\n";
			}
		}
		catch (const boost::system::system_error& e)
		{
			throw std::runtime_error("Failed to connect to server: " + std::string(e.what()));
		}
	}

	/**
	 * @brief Writes data to the server.
	 * @param data A span of characters containing the data to write.
	 */
	void Write(std::span<char> data) override
	{
		std::cout << __FUNCTION__ << std::endl;
		if (!is_connected_to_server_)
		{
			std::cerr << "Cannot send data, not connected to server.\n";
			return;
		}
		/*std::size_t copy_size = std::min(data.size(), buffer_.size());*/ // Ensure we don't exceed the buffer size

		/*std::cout << __FUNCTION__ << " copy_size: " << copy_size << std::endl;*/

		/*std::copy_n(data.begin(), copy_size, buffer_.begin());*/
		// Send the data to the server asynchronously
		// The lambda function handles the completion of the write operation
		if (!socket_.is_open()) {
			std::cerr << "Socket is not open. Cannot write data.\n";
			return;
		}
		boost::asio::async_write(socket_, boost::asio::buffer(data.data(), data.size()),
			[this](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					std::cout << "Sent " << bytes_transferred << "\n";
				}
				else
				{
					std::cerr << "Error during file transmission: " << ec.message() << "\n";
					socket_.close();
				}
			});
	}
	/**
  * @brief Stops the writing process.
  */
	void Stop() override
	{
		is_connected_to_server_ = false;
	}

	/**
  * @brief Checks if the writing process is running.
  * @return True if running, false otherwise
  */
	bool IsRunning() override
	{
		std::cout << __FUNCTION__ << std::endl;
		return is_connected_to_server_;
	}
private:
	/*std::array<char, 1024> buffer_;*/
	boost::asio::io_context io_context_;///<to initiate and manage asynchronous operations, such as connecting to a server, reading, or writing data over a network.
	tcp::socket socket_{ io_context_ };  ///< TCP socket for communication
	tcp::resolver resolver_{ io_context_ };  ///< Resolver for DNS resolution
	std::atomic<bool> is_connected_to_server_{ false };  ///< Thread-safe flag to indicate if data can be sent
};
