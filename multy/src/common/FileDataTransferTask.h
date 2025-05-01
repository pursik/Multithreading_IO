#pragma once

#include "ITask.h"
#include "IReader.h"
#include "IWriteDataAccess.h"
#include "Logger.h"

/**
 * @class ReadManager
 * @brief A task that reads data from a reader and writes it to a sync data access.
 */
class FileDataTransferTask : public ITask
{
private:
	std::shared_ptr<IWriteDataAccess> syncDataAccess_; ///< Shared data access for writing data of file to somewhere
	std::shared_ptr<IReader> reader_; ///< Reader to read data from file
	Logger logger_; ///< Logger instance

public:
	/**
	 * @brief Constructor for FileReaderBufferWriter task.
	 * @param syncSharedDataAccess Shared data access for writing
	 * @param reader Reader to read data from file
	 */
	FileDataTransferTask(std::shared_ptr<IWriteDataAccess> syncDataAccess, std::shared_ptr<IReader> reader)
		: syncDataAccess_(syncDataAccess)
		, reader_(reader)
		, logger_("FileDataTransferTask")
	{
	}

	/**
	 * @brief Runs the reading task.
	 * @throws std::invalid_argument if the reader is not set
	 */
	void Run() override
	{
		if (reader_ == nullptr)
		{
			throw std::invalid_argument("Reader is not set.");
		}
		try
		{
			auto data = reader_->Read();
			while (!data.empty())
			{
				syncDataAccess_->Write({ data });
				//	throw std::runtime_error("throw any exception code after complete of first memory block writing");
				data = reader_->Read();
			}
			syncDataAccess_->Stop(); // there is no more data to write into shared area
		}
		catch (std::runtime_error& e)
		{
			syncDataAccess_->Stop(); // there is no ability to write into shared area
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
};
