#pragma once

#include "../common/IWriter.h"
#include "../common/Logger.h"
#include "../common/DataType.h"
#include "../FileStream/FileOperationFactory.h"
#include <memory>
#include <vector>
#include <filesystem>
#include <boost/asio.hpp>

class FileWriter : public IWriter, public std::enable_shared_from_this<FileWriter>
{
private:
	Logger logger_;
	FileWriter(size_t ID) :ID_(ID), logger_("FileWriter " + std::to_string(ID)) {};
	std::shared_ptr<IWriter> fileDataWriter_ = nullptr;
	size_t ID_; ///< Unique identifier 
public:
	~FileWriter() = default;
	/**
	 * @brief Creates a new FileWriter instance.
	 * @param ID is the unique identifier for the connection.
	 * @return A shared pointer to the created TCPConnection instance.
	 */
	static std::shared_ptr<IWriter> Create(size_t ID)
	{
		return std::shared_ptr<FileWriter>(new FileWriter(ID));
	}

	void Write(std::span<char> data) override
	{
		if (data.empty())
		{
			return;
		}
		auto s = data.size();
		// Extract the data type from the first byte
		const auto dataType = static_cast<std::uint8_t>(data[0]);

		// Handle the data based on its type
		switch (static_cast<DataType>(dataType))
		{
		case DataType::Type_File_Name:
			// Process the file name data (excluding the first byte)
			HandleFileName(data.subspan(1));
			break;

		case DataType::Type_File_Data:
			// Process the file data (excluding the first byte)
			HandleFileData(data.subspan(1));
			break;
		default:
			break;
		}
	}

	void HandleFileName(std::span<char> fileNameData)
	{
		std::string fileName(fileNameData.begin(), fileNameData.end());
		std::cout << __FUNCTION__ << "  " <<std::string(fileName) << "\n";
		fileDataWriter_ = FileOperationFactory::CreateFileDataWriter(fileName, "client_" + std::to_string(ID_));
	}

	void HandleFileData(std::span<char> fileData)
	{
		std::cout << __FUNCTION__ << "\n";
		if (!fileDataWriter_)
		{
			std::cerr << "File stream is not initialized. Cannot write file data.";
			return;
		}
		try
		{
			fileDataWriter_->Write(fileData);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error writing file data: " << e.what() << "\n";
		}
	}
};
