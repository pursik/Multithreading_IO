#pragma once

#include "TCPConnection.h"
#include "FileWriter.h"
#include "../common/Logger.h"
#include "../common/ITask.h"

#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

/**
 * @class TCPServer
 * @brief A class that represents a TCP server which listens for incoming client connections and reads data.
 */
class TCPServer : public ITask
{
public:
	/**
	 * @brief Constructs a TCPServer object.
	 * @param io_context The io_context object used for asynchronous operations.
	 */
	explicit TCPServer(uint64_t port, boost::asio::io_context& io_context)
		: logger_("TCPServer")
		, io_context_(io_context)
		, acceptor_(io_context, tcp::endpoint(tcp::v4(), port))  // Listen on port 13
	{
	}
	void Run() override
	{
		// Create a new TCPConnection instance
		++current_id_;
		auto client_connection = TCPConnection::Create(io_context_, current_id_, FileWriter::Create(current_id_));// Create a new TCPConnection instance

		// Begin asynchronously accepting new connections
		acceptor_.async_accept(
			client_connection->Socket(),
			[this, client_connection](boost::system::error_code error)
			{
				if (!error)
				{
					std::cout << "Accepted new connection ID:" << client_connection->GetID() << std::endl;

					client_connection->Read();//reading from the client
					Run(); // Start accepting the next connection
				}
				else
				{
					std::cerr << "Error during accepting connection: " << error.message() << "\n";
				}
			});
	}

private:
	tcp::acceptor acceptor_;  ///< Acceptor to listen for incoming client connections
	boost::asio::io_context& io_context_;  ///< Reference to the io_context object
	Logger logger_; ///< Logger instance for logging
	size_t current_id_{ 0 };
	std::vector<TCPConnection::pointer> client_connections_; // Track active client connection
};


