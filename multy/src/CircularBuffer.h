#pragma once

#include "IBuffer.h"
#include "IStateSetter.h"
#include <vector>
#include <memory>
#include <iostream>
#include <windows.h>
class CircularBuffer : public IBuffer
{
private:
	char* data_;           // Raw buffer pointer from shared memory
	std::vector<char> dataVec_;  // Used only in in-memory mode
	const size_t capacity_;
	std::shared_ptr<IStateSetter> state_;

public:
	CircularBuffer(size_t size, std::shared_ptr<IStateSetter> state, void* mapViewOfFile = nullptr) :
		capacity_(size), state_(state)
	{
		if (mapViewOfFile == nullptr)
		{
			dataVec_.resize(size); // Ensure dataVec_ is properly resized
			data_ = dataVec_.data(); // Update data_ to point to the resized vector
		}
		else
		{
			data_ = static_cast<char*>(mapViewOfFile);
		}
	}
	~CircularBuffer() = default;

	char& operator[](size_t index) override
	{
		return data_[index % capacity_]; // Circular access
	}

	size_t GetDataSize() const override
	{
		return head_ < tail_
			? tail_ - head_
			: capacity_ - head_ + tail_; // Handle wrap around
	}

	void Write(char item) override
	{
		/*MessageBoxA(NULL, "Write", "Write", MB_OK);*/
		data_[tail_] = item;
		
		tail_ = (tail_ + 1) % capacity_;

		state_->EnableReading();

		if (tail_ == head_)
		{
			state_->DisableWriting();
		}
	}

	size_t GetHead() const override
	{
		return head_;
	}

	void SetHead(size_t value) override
	{
		head_ = (head_ + value) % capacity_;
		state_->EnableWriting();
		if (head_ == tail_)
		{
			state_->DisableReading();
		}
	}
};

