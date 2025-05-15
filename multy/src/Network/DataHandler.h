#pragma once

#include "../common/IWriter.h"
#include "../common/Logger.h"
#include "../common/Serializer.h"
#include "../FileStream/FileOperationFactory.h"
#include "Encryption.h"
#include <memory>
#include <vector>
#include <filesystem>
#include <boost/asio.hpp>

class DataHandler : public IWriter, public std::enable_shared_from_this<DataHandler>
{
private:
	DataHandler(size_t ID, std::shared_ptr<IEncrypt> encryptor) :
		logger_("DataHandler " + std::to_string(ID)), 
		ID_(ID), 
		serializer_(std::move(encryptor)) 
	{};

	Logger logger_;
	size_t ID_; ///< Unique identifier 
	Serializer serializer_;///< Serializer to decrypt data
	std::shared_ptr<IWriter> dataWriter_ = nullptr; ///< Pointer to an IWriter object for file writing

public:
	~DataHandler() = default;
	/**
	 * @brief Creates a new FileWriter instance.
	 * @param ID is the unique identifier for the connection.
	 * @return A shared pointer to the created TCPConnection instance.
	 */
	static std::shared_ptr<IWriter> Create(size_t ID, std::shared_ptr<IEncrypt> encryptor)
	{
		return std::shared_ptr<DataHandler>(new DataHandler(ID, encryptor));
	}

	void Write(std::span<char> data) override
	{
		auto recoveredData = serializer_.Unpack(data);
		if (recoveredData.empty())
		{
			throw std::invalid_argument("Error: Unpacking data failed.");
		}
		// Handle the data based on its type
		switch (serializer_.getType())
		{
		case DataType::Type_File_Name:
			// Process the file name data (excluding the first byte)
			HandleFileName(recoveredData);
			break;

		case DataType::Type_File_Data:
			// Process the file data (excluding the first byte)
			HandleFileData(recoveredData);
			break;
		default:
			break;
		}
	}

	void HandleFileName(std::span<char> fileNameData)
	{
		std::string fileName(fileNameData.begin(), fileNameData.end());
		std::cout << __FUNCTION__ << "  " << std::string(fileName) << "\n";
		if (fileName.empty())
		{
			throw std::invalid_argument("Error: File name cannot be empty.");
		}
		dataWriter_ = FileOperationFactory::CreateFileDataWriter(fileName, "client_" + std::to_string(ID_));
	}

	void HandleFileData(std::span<char> fileData)
	{
		std::cout << __FUNCTION__ << "\n";
		try
		{
			dataWriter_->Write(fileData);
		}
		catch (const std::exception& e)
		{
			throw std::invalid_argument(e.what());
		}
	}
};
