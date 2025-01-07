#ifndef BUFFER_PROC_H
#define BUFFER_PROC_H

#include "IWriter.h"
#include "IReader.h"
#include "Buffer.h"

#include <vector>

class BufferReader : public IReader
{
private:
	std::shared_ptr<BaseBuffer> buffer_;
	std::vector<char> chunk_;

public:
	BufferReader(std::shared_ptr<BaseBuffer> buffer) : buffer_(buffer) {}
	std::span<char> Read() override
	{
		size_t dataSize = buffer_->GetDataSize();

		chunk_.clear();
		chunk_.resize(dataSize);
		const auto head = buffer_->GetHead();
		for (size_t i = 0; i < dataSize; ++i)
		{
			chunk_[i] = (*buffer_)[head + i];
		}
		buffer_->SetHead(dataSize);
		return std::span<char>(chunk_.data(), chunk_.size());
	}
};

class BufferWriter : public IWriter
{
private:
	std::shared_ptr<BaseBuffer> buffer_;

public:
	BufferWriter(std::shared_ptr<BaseBuffer> buffer) : buffer_(buffer) {}
	void Write(std::span<char> data) override
	{
		for (const char value : data)
		{
			buffer_->Write(value);
		}
	}
};

#endif
