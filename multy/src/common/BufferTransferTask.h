#pragma once

#include "ITask.h"
#include "IReadDataAccess.h"
#include "IWriter.h"
#include "Logger.h"

/**
 * @class WriteManager
 * @brief A task that reads data from a sync data access and writes it to a writer.
 */
class BufferTransferTask : public ITask
{
private:
	std::shared_ptr<IReadDataAccess> syncDataAccess_; ///< Shared data access for reading
	std::shared_ptr<IWriter> writer_; ///< Writer to write data to
	Logger logger_; ///< Logger instance

public:
	/**
	 * @brief Constructor for WriteManager task.
	 * @param syncSharedDataAccess Shared data access for reading
	 * @param writer Writer to write data to
	 */
	BufferTransferTask(std::shared_ptr<IReadDataAccess> syncDataAccess, std::shared_ptr<IWriter> writer)
		: syncDataAccess_(syncDataAccess)
		, writer_(writer)
		, logger_("WriteManager")
	{
	}

	/**
	 * @brief Runs the writing task.
	 * @throws std::invalid_argument if the writer is not set
	 */
	void Run() override
	{
		if (writer_ == nullptr)
		{
			throw std::invalid_argument("Writer is not set.");
		}
		try
		{
			while (syncDataAccess_->IsRunning())
			{
				const auto data = syncDataAccess_->Read();
				if (!data.empty())
				{
					//throw std::runtime_error("throw any exception code after complete of first memory block reading");
					writer_->Write(data);
				}
			}
		}
		catch (std::runtime_error& e)
		{
			syncDataAccess_->Stop(); // there is no ability to write into shared area
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
};
