#ifndef BUFFER_H
#define BUFFER_H

#include "IBufferStateWriter.h"
#include <vector>
#include <memory>

struct BaseBuffer
{
	virtual ~BaseBuffer() = default;
	virtual char& operator[](size_t index) = 0;
	virtual void Write(char item) = 0;
	virtual size_t GetDataSize() const = 0;
	virtual size_t GetHead() const = 0;
	virtual void SetHead(size_t value) = 0;

protected:
	size_t head_ = 0;
	size_t tail_ = 0;
};

class CircularBuffer : public BaseBuffer
{
private:
	std::vector<char> data_;
	const size_t capacity_;
	std::shared_ptr<IBufferStateWriter> state_;
public:
	CircularBuffer(size_t size, std::shared_ptr<IBufferStateWriter> state) : data_(size), capacity_(size), state_(state) {}
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
#endif
