#include "buffer.h"

void Buffer::Refill(const std::vector<char> &data)
{
	std::unique_lock<std::mutex> locker(mutex_);
	queue_.push(data);
	condition_.notify_one();
}

std::vector<char> Buffer::Retrieve()
{
	std::unique_lock<std::mutex> locker(mutex_);
	condition_.wait(locker, [this]
					{ return !queue_.empty() || stopWritingToBuffer_; });
	if (!queue_.empty())
	{
		auto data = queue_.front();
		queue_.pop();
		return data;
	}
	return {};
}

bool Buffer::Empty()
{
	std::unique_lock<std::mutex> locker(mutex_);
	return queue_.empty();
}

void Buffer::StopBufferRefilling()
{
	std::unique_lock<std::mutex> locker(mutex_);
	stopWritingToBuffer_ = true;
	condition_.notify_all();
}

bool Buffer::IsBufferRefillingStopped()
{
	std::unique_lock<std::mutex> locker(mutex_);
	return stopWritingToBuffer_;
}
