#pragma once

#include "IProcessing.h"
#include "IDataAccess.h"

class Processing : public IProcessing
{
private:
	std::shared_ptr<IDataAccess> syncSharedDataAccess_;
	std::shared_ptr<IReader> reader_;
	std::shared_ptr<IWriter> writer_;

public:
	Processing(std::shared_ptr<IDataAccess> syncSharedDataAccess, std::shared_ptr<IReader> reader, std::shared_ptr<IWriter> writer) : syncSharedDataAccess_(syncSharedDataAccess), reader_(reader), writer_(writer)
	{
	}

	void Reading()
	{
		if (reader_ == nullptr)
		{
			throw std::invalid_argument("Reader is not set.");
		}
		auto data = reader_->Read();
		while (!data.empty())
		{
			syncSharedDataAccess_->Write({ data });
			data = reader_->Read();
		}
		syncSharedDataAccess_->Stop(); // there is no more data to write into shared area
	}

	void Writing()
	{
		if (writer_ == nullptr)
		{
			throw std::invalid_argument("Writer is not set.");
		}
		bool isRunning = syncSharedDataAccess_->IsRunning();
		while (isRunning)
		{
			writer_->Write(syncSharedDataAccess_->Read());
			syncSharedDataAccess_->NotifyBufferNotFull();
			isRunning = syncSharedDataAccess_->IsRunning();
		}
	}
};

