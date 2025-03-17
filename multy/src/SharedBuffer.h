#pragma once

#include "IBuffer.h"
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
namespace bip = boost::interprocess;
struct SharedData 
{
	bip::interprocess_mutex mutex;
	bip::interprocess_condition cond_empty;
	bip::interprocess_condition cond_full;
	size_t capacity;
	size_t head = 0;
	size_t tail = 0;
	char buffer[1]; // flexible array member, length defined at runtime
	SharedData(size_t size) : capacity(size), head(0), tail(0) {}
};

class SharedCircularBuffer : public IBuffer {
private:
	bip::managed_shared_memory segment_;
	SharedData* data_;

public:
	SharedCircularBuffer(size_t size)
		: segment_(bip::open_or_create, "CircularBufferSharedMemory", sizeof(SharedData) + size - 1)
	{
		data_ = segment_.find_or_construct<SharedData>("BufferElements")(size);
	}

	char& operator[](size_t index) override
	{
		return data_->buffer[index % data_->capacity];
	}

	void Write(char item) override
	{
		bip::scoped_lock<bip::interprocess_mutex> lock(data_->mutex);
		while ((data_->tail + 1) % data_->capacity == data_->head) { // buffer full
			data_->cond_full.wait(lock);
		}
		data_->buffer[data_->tail] = item;
		data_->tail = (data_->tail + 1) % data_->capacity;
		data_->cond_empty.notify_one();
	}

	size_t GetHead() const override 
	{ 
		return data_->head;
	}

	void SetHead(size_t value) override 
	{
		bip::scoped_lock<bip::interprocess_mutex> lock(data_->mutex);
		data_->head = (data_->head + value) % data_->capacity;
		data_->cond_full.notify_one();
	}
	size_t GetDataSize() const override 
	{
		return (data_->tail >= data_->head)?
			data_->tail - data_->head: data_->capacity - (data_->head - data_->tail);
	}
};


