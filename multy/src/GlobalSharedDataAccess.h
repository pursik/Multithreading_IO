#pragma once

#include "IStateGetter.h"
#include "IDataAccess.h"
#include <windows.h>

#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

class GlobalSharedDataAccess : public IDataAccess
{
private:
	HANDLE semaphor_ = nullptr;
	std::shared_ptr<IWriter> writer_;
	std::shared_ptr<IReader> reader_;
	std::shared_ptr<IStateGetter> bufferState_;
	HANDLE hStopEvent_;

public:
	GlobalSharedDataAccess(std::shared_ptr<IWriter> writer, std::shared_ptr<IReader> reader, std::shared_ptr<IStateGetter> state, HANDLE semaphor) :
		writer_(writer), reader_(reader), bufferState_(state), semaphor_(semaphor)
	{
		hStopEvent_ = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Global\\DataStopEvent_");
		if (hStopEvent_ == NULL)
		{
			hStopEvent_ = CreateEvent(NULL, FALSE, FALSE, "Global\\hStopEvent_");
			if (hStopEvent_ == NULL)
			{
				throw std::runtime_error("Error: Could not open stop event.");
			}
		}
	}

	~GlobalSharedDataAccess()
	{
		CloseHandle(semaphor_);
		CloseHandle(hStopEvent_);
	}

	void Write(std::span<char> data) override
	{
		bool isWritingEnabled = bufferState_->IsWritingEnabled();
		bool isRunning = IsRunning();

		if (isWritingEnabled && isRunning)
		{
			WaitForSingleObject(semaphor_, INFINITE);  // Wait for buffer space 
			writer_->Write(data);
			ReleaseSemaphore(semaphor_, 1, nullptr);  // Signal data is available
		}
	}

	std::span<char> Read() override
	{
		if (bufferState_->IsReadingEnabled())
		{
			WaitForSingleObject(semaphor_, INFINITE);  // Wait for data
			return reader_->Read();
		}
		return {};
	}

	void NotifyBufferNotFull() override
	{
		ReleaseSemaphore(semaphor_, 1, NULL);
	}

	void Stop() override
	{
		SetEvent(hStopEvent_);
	}

	bool IsRunning() override
	{
		auto result = WaitForSingleObject(hStopEvent_, 0); // Use 0 to check the state without waiting
		return (result == WAIT_TIMEOUT); // If the event is not signaled, the process is running
	}
};
