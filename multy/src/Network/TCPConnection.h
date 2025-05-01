#pragma once

#include "../common/IReader.h"
#include "../common/IWriter.h"
#include "../common/DataBuffer.h"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "../common/Logger.h"

using boost::asio::ip::tcp;

/**
 * @class TCPConnection
 * @brief Manages a TCP connection and provides asynchronous read functionality.
 */
class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public:
	using pointer = std::shared_ptr<TCPConnection>;
	using cleaner = std::function<void(size_t)>;
	~TCPConnection() = default; ///< Default destructor

	/**
  * @brief Creates a new TCPConnection instance.
  * @param io_context The io_context object used for asynchronous operations.
  * @param ID The unique identifier for the connection.
  * @return A shared pointer to the created TCPConnection instance.
  */
	static pointer Create(boost::asio::io_context& io_context, size_t ID, std::shared_ptr<IWriter> fileWriter)
	{
		return std::shared_ptr<TCPConnection>(new TCPConnection(io_context, ID, std::move(fileWriter)));
	}

	/**
  * @brief Gets the socket associated with the connection.
  * @return A reference to the socket.
  */
	tcp::socket& Socket()
	{
		return socket_;
	}

	/**
  * @brief Reads data asynchronously from the socket.
  */
	bool Read()
	{
		auto self = shared_from_this();
		socket_.async_read_some(
			boost::asio::buffer(buffer_.data_),
			[self](const boost::system::error_code& error, std::size_t length)
			{
				if (!error)
				{
					std::cout << "Message received from client " << self->ID_ << ": "
						<< std::string(self->buffer_.data_.data(), length) << "\n";

					// Process the chunk of data
					self->fileWriter_->Write({ self->buffer_.data_.data(), length });

					// Continue reading the next chunk
					self->Read();
				}
				else if (error == boost::asio::error::eof)
				{
					std::cout << "Client disconnected ID: " << self->GetID() << "\n";
					self->socket_.close();
				}
				else if (error == boost::asio::error::operation_aborted)
				{
					std::cout << "Operation aborted\n";
				}
				else
				{
					std::cerr << "Error while receiving message: " << error.message() << "\n";
				}
			});
		std::cout << __FUNCTION__ << "..."<< socket_.is_open() << " \n";
		return socket_.is_open();
	}

	/**
  * @brief Gets the unique identifier for the connection.
  * @return The unique identifier.
  */
	size_t GetID() const
	{
		return ID_;
	}

private:
	/**
  * @brief Constructor for TCPConnection.
  * @param io_context The io_context object used for asynchronous operations.
  * @param ID The unique identifier for the connection.
  */
	explicit TCPConnection(boost::asio::io_context& io_context, size_t ID, std::shared_ptr<IWriter> fileWriter)
		: logger_("TCPConnection " + std::to_string(ID)),
		socket_(io_context),
		buffer_(), // Explicitly initialize the buffer_
		ID_(ID),
		fileWriter_(fileWriter) // Create a writer instance
	{
	}

	tcp::socket socket_; ///< The socket associated with the connection.
	DataBuffer buffer_; ///< Buffer to store the read data.
	size_t ID_; ///< Unique identifier for the connection.
	std::shared_ptr<IWriter> fileWriter_; ///< The file writer.
	Logger logger_; ///< Logger instance for logging
};
