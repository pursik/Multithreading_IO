#ifndef BUFFER_H
#define BUFFER_H

#include "buffer_interface.h"
#include <mutex>
#include <condition_variable>
#include <queue>

class Buffer : public IBuffer
{
private:
	std::queue<std::vector<char>> queue_;
	std::mutex mutex_;
	std::condition_variable condition_;
	bool stopWritingToBuffer_ = false;

public:
	void Refill(const std::vector<char> &data) override;

	std::vector<char> Retrieve() override;

	bool Empty() override;

	void StopBufferRefilling() override;

	bool IsBufferRefillingStopped() override;
};

#endif
