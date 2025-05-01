#pragma once

#include "../common/IReader.h"
#include "../common/Logger.h"
#include "../common/DataType.h"
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
	static constexpr uint8_t header = static_cast<uint8_t>(DataType::Type_File_Data);
	std::vector<char> buffer_; ///< Buffer to store the header and file data.
	/**
  * @brief Private constructor to prevent direct construction.
  * @param reader file data reader
  */
	FileDataSegmentReader(std::shared_ptr<IReader> reader)
		:reader_(std::move(reader)), logger_("FileDataSegmentReader")
	{}

	void ToByteArray(std::span<char> data)
	{
		if (!data.empty())
		{
			buffer_.reserve(1 + data.size());

			// Add header to the buffer
			buffer_.push_back(header);

			// Add the content of the file data
			buffer_.insert(buffer_.end(), data.begin(), data.end());
		}
	}
public:
	/**
  * @brief Creates an FileDataSegmentReader instance.
  * @param fileName Name of the file to read from
  * @param chunkSize Size of the chunk to read
  * @return Shared pointer to the created FileDataSegmentReader instance
  * @throws std::runtime_error if the file cannot be opened
  */
	static std::shared_ptr<IReader> Create(std::shared_ptr<IReader> reader)
	{
		return std::shared_ptr<FileDataSegmentReader>(new FileDataSegmentReader(std::move(reader)));
	}

	/**
  * @brief Reads a chunk of data from the file.
  * @return A span of characters containing the read data
  */
	std::span<char> Read() override
	{
		buffer_.clear(); // Clear the buffer before use to avoid appending to old data
		ToByteArray(reader_->Read());
		return { buffer_.data(), buffer_.size() };
	}
};

