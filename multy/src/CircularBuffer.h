#pragma once

#include "IBuffer.h"
#include "IStateSetter.h"
#include <vector>
#include <memory>

class CircularBuffer : public IBuffer
{
private:
	std::vector<char> data_;
	const size_t capacity_;
	std::shared_ptr<IStateSetter> state_;
public:
	CircularBuffer(size_t size, std::shared_ptr<IStateSetter> state) : data_(size), capacity_(size), state_(state) {}
	~CircularBuffer() = default;

	char& operator[](size_t index) override
	{
		return data_[index % capacity_]; // Circular access
	}

	size_t GetDataSize() const override
	{
		return head_ < tail_
			? tail_ - head_
			: data_.size() - head_ + tail_; // Handle wrap around
	}

	void Write(char item) override
	{
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

