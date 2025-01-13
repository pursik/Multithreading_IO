#pragma once

#include "IWriter.h"
#include "IBuffer.h"
#include <memory>

class BufferWriter : public IWriter
{
private:
	std::shared_ptr<IBuffer> buffer_;

public:
	BufferWriter(std::shared_ptr<IBuffer> buffer) : buffer_(buffer) {}
	void Write(std::span<char> data) override
	{
		for (const char value : data)
		{
			buffer_->Write(value);
		}
	}
};
