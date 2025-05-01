#pragma once

#include "../common/ITask.h"
#include "../common/IReader.h"
#include "../common/IWriter.h"
#include "Logger.h"

class FileNameTransferTask : public ITask
{
private:
	std::shared_ptr<IReader> reader_; ///< task to read name of file
	std::shared_ptr<IWriter> writer_; ///< task to write name of file
	Logger logger_; ///< Logger instance

public:
	FileNameTransferTask(std::shared_ptr<IWriter> writer, std::shared_ptr<IReader> reader)
		: reader_(reader)
		, writer_(writer)
		, logger_("FileNameTransferTask")
	{
	}

	void Run() override
	{
		if (!reader_ || !writer_)
		{
			throw std::invalid_argument("Transfer is not possible.");
		}
		try
		{
			const auto data = reader_->Read();
			if (!data.empty())
			{
				writer_->Write(data);
			}
			else
			{
				std::cerr << "Error: No name to write.\n";
			}
		}
		catch (std::runtime_error& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
};
