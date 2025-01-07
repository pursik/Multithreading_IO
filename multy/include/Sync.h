#ifndef SYNC_H
#define SYNC_H

#include "IWriter.h"
#include "IReader.h"
#include "IBufferStateReader.h"

#include <mutex>
#include <condition_variable>
#include <functional>

class SafeDataAccess : public IWriter, IReader
{
private:
	std::mutex mutex_;
	std::condition_variable cv_;
	std::atomic<bool> running_ = true;
	std::shared_ptr<IWriter> writer_;
	std::shared_ptr<IReader> reader_;
	std::shared_ptr<IBufferStateReader> bufferState_;

public:
	SafeDataAccess(std::shared_ptr<IWriter> writer, std::shared_ptr<IReader> reader, std::shared_ptr<IBufferStateReader> state) :writer_(writer), reader_(reader), bufferState_(state) {}

	void Write(std::span<char> data) override
	{
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [&]()
			{ return bufferState_->IsWritingEnabled() && IsRunning(); });
		writer_->Write(data);
		cv_.notify_all();
	}

	std::span<char> Read() override
	{
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [&]()
			{ return bufferState_->IsReadingEnabled(); });
		return reader_->Read();
	}

	void NotifyBufferNotFull()
	{
		cv_.notify_all();
	}

	void Stop()
	{
		running_ = false;
	}

	bool IsRunning()
	{
		return running_;
	}
};
#endif
