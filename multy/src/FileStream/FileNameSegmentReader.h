#pragma once

#include "../common/IReader.h"
#include "../common/Logger.h"
#include "../common/Serializer.h"

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
	Logger logger_; ///< Logger instance
	std::string filename_;
	Serializer serializer_;///< Serializer to encrypt data

	/**
	* @brief Private constructor to prevent direct construction.
	*/
	FileNameSegmentReader(std::string_view fileName, std::shared_ptr<IEncrypt> encryptor) :
		filename_(fileName), 
		logger_("FileNameSegmentReader"),
		serializer_(std::move(encryptor), DataType::Type_File_Name)
	{
		if (filename_.empty())
		{
			throw std::invalid_argument("Error: File name cannot be empty.");
		}
		if (!fs::exists(filename_))
		{
			std::cout << filename_ << " does not exist." << std::endl;
			throw std::runtime_error(filename_ + " does not exist.");
		}
	}
public:
	/**
	* @brief Creates an FileNameSegmentReader instance.
	* @param fileName Name of the file to read from
	* @param encryptor
	* @return Shared pointer to the created FileNameSegmentReader instance
	*/
	static std::shared_ptr<IReader> Create(std::string_view fileName, std::shared_ptr<IEncrypt> encryptor)
	{
		std::cout << __FUNCTION__ << " ..." << std::string(fileName) << std::endl;
		return std::shared_ptr<FileNameSegmentReader>(new FileNameSegmentReader(fileName, encryptor));
	}

	/**
	* @brief Returns a file name.
	* @return A span of characters containing the header + serialzed file name
	*/
	std::span<char> Read() override
	{
		std::cout << __FUNCTION__ << std::endl;
		// Return a span pointing to the buffer
		return serializer_.Pack({ filename_.begin(), filename_.end() });
	}
};
