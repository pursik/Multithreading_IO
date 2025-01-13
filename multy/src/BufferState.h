#pragma once

#include "IStateSetter.h"
#include "IStateGetter.h"
#include <memory>


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

