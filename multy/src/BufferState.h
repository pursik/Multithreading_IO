#pragma once

#include "IStateSetter.h"
#include "IStateGetter.h"
#include <memory>
#include <windows.h>
#include <stdexcept>

class BufferState : public IStateSetter, public IStateGetter
{
public:
	BufferState() {}

	void EnableWriting() override
	{
		writingEnabled_ = true;
	}

	void EnableReading() override
	{
		readingEnabled_ = true;
	}

	void DisableWriting() override
	{
		writingEnabled_ = false;
	}

	void DisableReading() override
	{
		readingEnabled_ = false;
	}

	bool IsWritingEnabled() override
	{
		return writingEnabled_;
	}

	bool IsReadingEnabled() override
	{
		return readingEnabled_;
	}

private:
	std::atomic<bool> writingEnabled_ = true;
	std::atomic<bool> readingEnabled_ = false;
};

class InterProcessBufferState : public IStateSetter, public IStateGetter
{
public:
	InterProcessBufferState()
	{
		hDataAvailableEvent_ = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Global\\DataAvailableEvent");
		if (hDataAvailableEvent_ == NULL)
		{
			hDataAvailableEvent_ = CreateEvent(NULL, FALSE, FALSE, "Global\\DataAvailableEvent");
			if (hDataAvailableEvent_ == NULL)
			{
				throw std::runtime_error("Error: Could not open data available event.");
			}
		}

		hSpaceAvailableEvent_ = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Global\\SpaceAvailableEvent");
		if (hSpaceAvailableEvent_ == NULL)
		{
			hSpaceAvailableEvent_ = CreateEvent(NULL, FALSE, FALSE, "Global\\SpaceAvailableEvent");
			if (hSpaceAvailableEvent_ == NULL)
			{
				throw std::runtime_error("Error: Could not open data space event.");
			}
		}
	}
	~InterProcessBufferState()
	{
		CloseHandle(hDataAvailableEvent_);
		CloseHandle(hSpaceAvailableEvent_);
	}

	void EnableWriting() override
	{
		SetEvent(hSpaceAvailableEvent_);
	}

	void EnableReading() override
	{
		SetEvent(hDataAvailableEvent_);
	}

	void DisableWriting() override
	{
		ResetEvent(hSpaceAvailableEvent_);
	}

	void DisableReading() override
	{
		ResetEvent(hDataAvailableEvent_);
	}

	bool IsWritingEnabled() override
	{
		const auto result = WaitForSingleObject(hSpaceAvailableEvent_, INFINITE);
		return (result == WAIT_OBJECT_0);
	}

	bool IsReadingEnabled() override
	{
		const auto result = WaitForSingleObject(hDataAvailableEvent_, INFINITE);
		return (result == WAIT_OBJECT_0);
	}

private:
	HANDLE hDataAvailableEvent_;
	HANDLE hSpaceAvailableEvent_;
};

