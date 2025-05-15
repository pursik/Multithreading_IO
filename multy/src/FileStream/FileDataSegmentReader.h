#pragma once

#include "../common/IReader.h"
#include "../common/Logger.h"
#include "../common/Serializer.h"

#include <fstream>
#include <filesystem>

/**
 * @class FileDataSegmentReader
 * @brief A class that reads data from a file in chunks.
 */
class FileDataSegmentReader : public IReader
{
private:
	Logger logger_; ///< Logger instance
	std::shared_ptr<IReader> reader_; ///< Reader to read data from file
	Serializer serializer_;///< Serializer to encrypt data

	/**
  * @brief Private constructor to prevent direct construction.
  * @param reader file data reader
  */
	FileDataSegmentReader(std::shared_ptr<IReader> reader, std::shared_ptr<IEncrypt> encryptor):
		reader_(std::move(reader)), 
		logger_("FileDataSegmentReader"), 
		serializer_(std::move(encryptor), DataType::Type_File_Data)
	{}
public:
	/**
  * @brief Creates an FileDataSegmentReader instance.
  * @param fileName Name of the file to read from
  * @param chunkSize Size of the chunk to read
  * @return Shared pointer to the created FileDataSegmentReader instance
  * @throws std::runtime_error if the file cannot be opened
  */
	static std::shared_ptr<IReader> Create(std::shared_ptr<IReader> reader, std::shared_ptr<IEncrypt> encryptor)
	{
		return std::shared_ptr<FileDataSegmentReader>(new FileDataSegmentReader(std::move(reader), std::move(encryptor)));
	}

	/**
  * @brief Reads a chunk of data from the file.
  * @return A span of characters containing the read data
  */
	std::span<char> Read() override
	{
		return serializer_.Pack(reader_->Read());
	}
};

