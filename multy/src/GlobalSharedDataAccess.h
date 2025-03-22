#pragma once

#include "IStateGetter.h"
#include "IDataAccess.h"
#include "Logger.h"
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <span>

namespace bip = boost::interprocess;

class GlobalSharedDataAccess : public IDataAccess
{
private:
	std::shared_ptr<IWriter> writer_;
	std::shared_ptr<IReader> reader_;
	std::shared_ptr<IStateGetter> bufferState_;

	std::unique_ptr<bip::named_mutex> namedMutex_;
	std::unique_ptr<bip::named_semaphore> stopSemaphore_;
	std::unique_ptr<bip::named_condition> dataAvailableCondition_;
	std::unique_ptr<bip::named_condition> spaceAvailableCondition_;

	static constexpr auto mutexName = "GlobalMutex";
	static constexpr auto stopSemaphoreName = "GlobalStopCondition";
	static constexpr auto dataAvailableConditionName = "DataAvailableCondition";
	static constexpr auto spaceAvailableConditionName = "SpaceAvailableCondition";
	Logger logger_;

public:
	GlobalSharedDataAccess(std::shared_ptr<IWriter> writer, std::shared_ptr<IReader> reader, std::shared_ptr<IStateGetter> state) :
		writer_(writer), reader_(reader), bufferState_(state), logger_("GlobalSharedDataAccess")
	{
		try
		{
			namedMutex_ = std::make_unique<bip::named_mutex>(bip::open_or_create, mutexName);
			stopSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, stopSemaphoreName, 0);
			dataAvailableCondition_ = std::make_unique<bip::named_condition>(bip::open_or_create, dataAvailableConditionName);
			spaceAvailableCondition_ = std::make_unique<bip::named_condition>(bip::open_or_create, spaceAvailableConditionName);
		}
		catch (const bip::interprocess_exception& e)
		{
			throw std::runtime_error("Error: Could not create or open stop condition, mutex, or semaphore.\n");
		}
	}

	~GlobalSharedDataAccess()
	{
		Cleaner();
	}

	static void Cleaner()
	{
		std::cout << __FUNCTION__ << "\n";
		bip::named_semaphore::remove(stopSemaphoreName);
		bip::named_mutex::remove(mutexName);
		bip::named_condition::remove(dataAvailableConditionName);
		bip::named_condition::remove(spaceAvailableConditionName);
	}

	void Write(std::span<char> data) override
	{
		bool isWritingEnabled = bufferState_->IsWritingEnabled();
		bool isRunning = IsRunning();
		std::cout << "Write  isWritingEnabled.." << isWritingEnabled << " isRunning..." << isRunning << std::endl;
		if (isWritingEnabled && isRunning)
		{
			bip::scoped_lock<bip::named_mutex> lock(*namedMutex_);
			writer_->Write(data);
			dataAvailableCondition_->notify_all();
		}
	}

	std::span<char> Read() override
	{
		std::cout << __FUNCTION__ << std::endl;
		if (!bufferState_->IsReadingEnabled())
		{
			bip::scoped_lock<bip::named_mutex> lock(*namedMutex_);
			std::cout << "dataAvailableCondition_->wait(lock)" << std::endl;
			dataAvailableCondition_->wait(lock);
		}
		auto data = reader_->Read();
		spaceAvailableCondition_->notify_all();
		return data;
	}

	void NotifyBufferNotFull() override
	{
		std::cout << __FUNCTION__ << std::endl;
		std::cout << " spaceAvailableCondition_->notify_all()" << std::endl;
		spaceAvailableCondition_->notify_all();
	}

	void Stop() override
	{
		std::cout << __FUNCTION__ << std::endl;
		stopSemaphore_->post();
	}

	bool IsRunning() override
	{
		return !stopSemaphore_->try_wait();
	}
};
