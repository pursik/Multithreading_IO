#pragma once

#include "ITask.h"
#include "Logger.h"

class FileTransferTask : public ITask
{
private:
	std::shared_ptr<ITask> nameTransfer_; ///< task to write name of file
	std::shared_ptr<ITask> dataTransfer_; ///< task to write data of file
	Logger logger_; ///< Logger instance

public:
	FileTransferTask(std::shared_ptr<ITask> nameTransfer, std::shared_ptr<ITask> dataTransfer)
		: nameTransfer_(nameTransfer)
		, dataTransfer_(dataTransfer)
		, logger_("FileTransferTask")
	{
	}

	void Run() override
	{
		if (nameTransfer_ == nullptr || dataTransfer_ == nullptr)
		{
			throw std::invalid_argument("Transfer is not possible.");
		}
		try
		{
			nameTransfer_->Run(); // Start the name transfer
			dataTransfer_->Run(); // Start the data transfer
		}
		catch (std::runtime_error& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
};
