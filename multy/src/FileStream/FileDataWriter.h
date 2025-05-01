#pragma once

#include "../common/IWriter.h"
#include "../common/Logger.h"
#include <fstream>
#include <filesystem>

/**
 * @class FileDataWriter
 * @brief A class that writes data to a file.
 */
class FileDataWriter : public IWriter
{
private:
	std::ofstream stream_; ///< Output file stream
	Logger logger_; ///< Logger instance

	/**
  * @brief Private constructor to prevent direct construction.
  */
	FileDataWriter()
		: logger_("FileDataWriter")
	{}

public:
	/**
  * @brief Creates an FileDataWriter instance.
  * @param fileName Name of the file to write to
  * @return Shared pointer to the created FileDataWriter instance
  * @throws std::runtime_error if the file cannot be opened
  */
	static std::shared_ptr<IWriter> Create(std::string fileName, std::string_view subFolderName)
	{
		std::cout << __FUNCTION__ << " ..." << fileName << std::endl;
		auto instance = std::shared_ptr<FileDataWriter>(new FileDataWriter());
		if (!subFolderName.empty())
		{
			const auto currentDir = std::filesystem::current_path();
			const auto subFolderPath = currentDir / subFolderName;
			std::filesystem::path fileNameAsPath = fileName;
			const auto fileNamePath = subFolderPath / fileNameAsPath.filename();

			// Check if the subfolder already exists, and if not, create it
			if (!std::filesystem::exists(subFolderPath)) 
			{
				std::filesystem::create_directory(subFolderPath);
				std::cout << "Subfolder created: " << subFolderPath << std::endl;
			}
			else 
			{
				std::cout << "Subfolder already exists: " << subFolderPath << std::endl;
			}
			fileName = fileNamePath.string();
		}
		instance->stream_.open(fileName, std::ios::binary);
		if (!instance->stream_.is_open())
		{
			if (std::filesystem::exists(fileName.data()))
			{
				throw std::runtime_error(fileName + " is failed to open.");
			}
			else
			{
				throw std::runtime_error(fileName + " does not exist.");
			}
		}
		return instance;
	}

	/**
  * @brief Writes a span of data to the file.
  * @param data A span of characters containing the data to write
  */
	void Write(std::span<char> data) override
	{
		if (stream_.is_open() && !data.empty())
		{
			stream_.write(data.data(), data.size());
			stream_.flush();
		}
	}
};
