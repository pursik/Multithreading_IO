#pragma once

#include "../common/IReader.h"
#include "../common/Logger.h"
#include "../common/DataType.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @class FileNameSegmentReader
 * @brief A class that reads file name.
 */
class FileNameSegmentReader : public IReader
{
private:
	static constexpr uint8_t header = static_cast<uint8_t>(DataType::Type_File_Name);
	Logger logger_; ///< Logger instance
	const std::string& filename_;
	std::vector<char> buffer_;///< Buffer to store the header and file name.
	/**
  * @brief Private constructor to prevent direct construction.
  */
	FileNameSegmentReader(const std::string& fileName) : filename_(fileName), logger_("FileNameSegmentReader")
	{}

	/**
	 * @brief Converts a file name into a byte array with a header and size prefix.
	 *
	 * This method serializes the file name into a buffer by adding a header byte,
	 * the file name itself.
	 *
	 * @param file_name The name of the file to be converted into a byte array.
	 * @throws std::invalid_argument If the file name is empty.
	 * @throws std::runtime_error If the file does not exist.
	 */
	void NameToBufferOfByte()
	{
		if (filename_.empty())
		{
			throw std::invalid_argument("Error: File name cannot be empty.");
		}
		if (!fs::exists(filename_))
		{
			throw std::runtime_error(filename_ + " does not exist.");
		}
		// Clear the buffer before use to avoid appending to old data
		buffer_.reserve(1 + filename_.size());

		// Add header to the buffer
		buffer_.push_back(header);

		// Add the content of the file_name
		buffer_.insert(buffer_.end(), filename_.begin(), filename_.end());

	}
public:
	/**
	* @brief Creates an FileNameSegmentReader instance.
	* @param fileName Name of the file to read from
	* @return Shared pointer to the created FileNameSegmentReader instance
	*/
	static std::shared_ptr<IReader> Create(const std::string& fileName)
	{
		std::cout << __FUNCTION__ << " ..." << std::string(fileName) << std::endl;
		return std::shared_ptr<FileNameSegmentReader>(new FileNameSegmentReader(fileName));
	}

	/**
	* @brief Returns a file name.
	* @return A span of characters containing the header + serialzed file name
	*/
	std::span<char> Read() override
	{
		std::cout << __FUNCTION__ << std::endl;
		// File name to buffer_ conversion
		NameToBufferOfByte();

		// Return a span pointing to the buffer
		return { buffer_.data(), buffer_.size() };
	}
};
