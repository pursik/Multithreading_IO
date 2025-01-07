#ifndef BUFFER_STATE_H
#define BUFFER_STATE_H

#include "IBufferStateWriter.h"
#include "IBufferStateReader.h"
#include <memory>

class BufferState : public IBufferStateWriter, public IBufferStateReader
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

#endif
