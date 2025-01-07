#ifndef WRITER_H
#define WRITER_H

#include "IWriter.h"
#include "Sync.h"

class IProcessing
{
public:
	virtual ~IProcessing() = default;
	virtual void Reading() = 0;
	virtual void Writing() = 0;
};

class Processing : public IProcessing
{
private:
	std::shared_ptr<SafeDataAccess> syncSharedDataAccess_;
	std::shared_ptr<IReader> reader_;
	std::shared_ptr<IWriter> writer_;

public:
	Processing(std::shared_ptr<SafeDataAccess> syncSharedDataAccess, std::shared_ptr<IReader> reader, std::shared_ptr<IWriter> writer) : syncSharedDataAccess_(syncSharedDataAccess), reader_(reader), writer_(writer)
	{
	}

	void Reading()
	{
		auto data = reader_->Read();
		while (!data.empty())
		{
			syncSharedDataAccess_->Write({data});
			data = reader_->Read();
		}
		syncSharedDataAccess_->Stop(); // there is no more data to write into shared area
	}

	void Writing()
	{
		while (syncSharedDataAccess_->IsRunning())
		{
			writer_->Write(syncSharedDataAccess_->Read());
			syncSharedDataAccess_->NotifyBufferNotFull();
		}
	}
};
#endif
