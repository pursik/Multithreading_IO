#pragma once

#include "IReader.h"
#include "IBuffer.h"
#include <vector>
#include <memory>

class BufferReader : public IReader
{
private:
	std::shared_ptr<IBuffer> buffer_;
	std::vector<char> chunk_;

public:
	BufferReader(std::shared_ptr<IBuffer> buffer) : buffer_(buffer) {}
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

