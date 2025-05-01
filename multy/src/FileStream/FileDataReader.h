#pragma once

#include "../common/IReader.h"
#include "../common/Logger.h"
#include <fstream>
#include <filesystem>
/**
 * @class FileDataReader
 * @brief A class that reads data from a file in chunks.
 */
class FileDataReader : public IReader
{
private:
	std::ifstream stream_; ///< Input file stream
	std::vector<char> chunkBuffer; ///< Buffer to store chunks of data
	Logger logger_; ///< Logger instance

	/**
  * @brief Private constructor to prevent direct construction.
  * @param chunkSize Size of the chunk to read
  */
	FileDataReader(size_t chunkSize)
		: chunkBuffer(chunkSize)
		, logger_("FileDataReader")
	{}

public:
	/**
  * @brief Creates an FileDataReader instance.
  * @param fileName Name of the file to read from
  * @param chunkSize Size of the chunk to read
  * @return Shared pointer to the created FileDataReader instance
  * @throws std::runtime_error if the file cannot be opened
  */
	static std::shared_ptr<IReader> Create(std::string_view fileName, size_t chunkSize)
	{
		std::cout << __FUNCTION__ << " ..." << std::string(fileName) << std::endl;
		auto instance = std::shared_ptr<FileDataReader>(new FileDataReader(chunkSize));
		instance->stream_.open(fileName.data(), std::ios::binary);
		if (!instance->stream_.is_open())
		{
			std::string error = std::string(fileName);
			if (std::filesystem::exists(fileName.data()))
			{
				throw std::runtime_error(std::string(fileName) + " is failed to open.");
			}
			else
			{
				throw std::runtime_error(std::string(fileName) + " does not exist.");
			}
		}
		return instance;
	}

	/**
  * @brief Reads a chunk of data from the file.
  * @return A span of characters containing the read data
  */
	std::span<char> Read() override
	{
		if ((stream_.read(chunkBuffer.data(), chunkBuffer.size()) /*false If the file reaches the end*/ || stream_.gcount() > 0 /*actually read amount*/))
		{
			const auto bytesRead = stream_.gcount(); /*returns the number of bytes that were actually read. <=dataSize */
			if (bytesRead)
			{
				chunkBuffer.resize(bytesRead);
			}
			return { chunkBuffer };
		}
		return {};
	}
};


