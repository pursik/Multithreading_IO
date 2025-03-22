#pragma once

#include "IStateSetter.h"
#include "IStateGetter.h"
#include "Logger.h"

#include <iostream>
#include <memory>
#include <atomic>
#include <boost/interprocess/sync/named_semaphore.hpp>

namespace bip = boost::interprocess;

class BufferState : public IStateSetter, public IStateGetter
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

class InterProcessBufferState : public IStateSetter, public IStateGetter
{
public:
	static constexpr auto dataAvailableSemaphoreName = "DataAvailableSemaphore";
	static constexpr auto spaceAvailableSemaphoreName = "SpaceAvailableSemaphore";

	InterProcessBufferState(): logger_("InterProcessBufferState")
	{
		dataAvailableSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, dataAvailableSemaphoreName, 0);
		spaceAvailableSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, spaceAvailableSemaphoreName, 0);
	}

	~InterProcessBufferState()
	{
		std::cout << __FUNCTION__ << "\n";
		Cleaner();
	}

	static void Cleaner()
	{
		std::cout << __FUNCTION__ << "\n";
		bip::named_semaphore::remove(dataAvailableSemaphoreName);
		bip::named_semaphore::remove(spaceAvailableSemaphoreName);
	}
	void EnableWriting() override
	{
		std::cout << "EnableWriting" << std::endl;
		spaceAvailableSemaphore_->post();
	}

	void EnableReading() override
	{
		std::cout << "EnableReading" << std::endl;
		dataAvailableSemaphore_->post();
	}

	void DisableWriting() override
	{
	}

	void DisableReading() override
	{
	}

	bool IsWritingEnabled() override
	{
		std::cout << "IsWritingEnabled ?" << std::endl;
		spaceAvailableSemaphore_->wait();
		std::cout << "IsWritingEnabled true" << std::endl;
		return true;
	}

	bool IsReadingEnabled() override
	{
		std::cout << "IsReadingEnabled ?" << std::endl;
		dataAvailableSemaphore_->wait();
		std::cout << "IsReadingEnabled true" << std::endl;
		return true;
	}

private:
	std::unique_ptr<bip::named_semaphore> dataAvailableSemaphore_;
	std::unique_ptr<bip::named_semaphore> spaceAvailableSemaphore_;
	Logger logger_;
};

