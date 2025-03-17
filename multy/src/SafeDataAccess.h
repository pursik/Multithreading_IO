#pragma once

#include "IStateGetter.h"
#include "IDataAccess.h"

#include <mutex>
#include <condition_variable>
#include <functional>

class SafeDataAccess : public IDataAccess
{
private:
	std::mutex mutex_;
	std::condition_variable cv_;
	std::atomic<bool> running_ = true;
	std::shared_ptr<IWriter> writer_;
	std::shared_ptr<IReader> reader_;
	std::shared_ptr<IStateGetter> bufferState_;

public:
	SafeDataAccess(std::shared_ptr<IWriter> writer, std::shared_ptr<IReader> reader, std::shared_ptr<IStateGetter> state) :writer_(writer), reader_(reader), bufferState_(state) {}

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

	void NotifyBufferNotFull() override
	{
		cv_.notify_all();
	}

	void Stop() override
	{
		running_ = false;
	}

	bool IsRunning() override
	{
		return running_;
	}
};


