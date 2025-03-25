#pragma once

#include "IStateSetter.h"
#include "IStateGetter.h"
#include "Logger.h"

#include <iostream>
#include <memory>
#include <atomic>
#include <boost/interprocess/sync/named_semaphore.hpp>

namespace bip = boost::interprocess;

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


