#pragma once

#include "IProcessing.h"
#include "IDataAccess.h"
#include "Logger.h"

class Processing : public IProcessing
{
private:
	std::shared_ptr<IDataAccess> syncSharedDataAccess_;
	std::shared_ptr<IReader> reader_;
	std::shared_ptr<IWriter> writer_;
	Logger logger_;
public:
	Processing(std::shared_ptr<IDataAccess> syncSharedDataAccess, std::shared_ptr<IReader> reader, std::shared_ptr<IWriter> writer)
		: syncSharedDataAccess_(syncSharedDataAccess)
		, reader_(reader)
		, writer_(writer)
		, logger_("Processing")
	{
	}

	void Reading()
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
				syncSharedDataAccess_->Write({ data });
				//	throw std::runtime_error("throw any exception code after complete of first memory block writing");
				data = reader_->Read();
			}
			syncSharedDataAccess_->Stop(); // there is no more data to write into shared area
		}
		catch (std::runtime_error& e)
		{
			syncSharedDataAccess_->Stop(); // there is no ability to write into shared area
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}

	void Writing()
	{
		if (writer_ == nullptr)
		{
			throw std::invalid_argument("Writer is not set.");
		}
		try
		{
			while (syncSharedDataAccess_->IsRunning())
			{
				const auto data = syncSharedDataAccess_->Read();
				if (!data.empty())
				{
					//throw std::runtime_error("throw any exception code after complete of first memory block reading");
					writer_->Write(data);
				}
			}
		}
		catch (std::runtime_error& e)
		{
			syncSharedDataAccess_->Stop(); // there is no ability to write into shared area
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
};

