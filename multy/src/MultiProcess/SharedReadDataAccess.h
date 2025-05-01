#pragma once

#include "../common/IReadDataAccess.h"
#include "../common/Logger.h"
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <mutex>
#include <iostream>
#include <memory>

namespace bip = boost::interprocess;

// Class to handle reading data from shared memory
class SharedReadDataAccess : public IReadDataAccess
{
private:
	constexpr static auto mutexName = "GlobalMutex";
	constexpr static auto stopSemaphoreName = "GlobalStopSemaphore";
	constexpr static auto dataAvailableSemaphoreName = "DataAvailableSemaphore";
	constexpr static auto spaceAvailableSemaphoreName = "SpaceAvailableSemaphore";

	std::shared_ptr<IReader> handler_; // Handler to read data
	std::unique_ptr<bip::named_semaphore> stopSemaphore_; // Semaphore to signal stop
	std::unique_ptr<bip::named_semaphore> dataAvailableSemaphore_; // Semaphore to signal data availability
	std::unique_ptr<bip::named_semaphore> spaceAvailableSemaphore_; // Semaphore to signal space availability
	Logger logger_; // Logger instance

public:
	// Constructor: Initializes the semaphores and handler
	SharedReadDataAccess(std::shared_ptr<IReader> handler) :
		handler_(std::move(handler)), logger_("SharedReadDataAccess")
	{
		try
		{
			stopSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, stopSemaphoreName, 0);
			dataAvailableSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, dataAvailableSemaphoreName, 0);
			spaceAvailableSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, spaceAvailableSemaphoreName, 1);
		}
		catch (const bip::interprocess_exception& e)
		{
			throw std::runtime_error(e.what());
		}
	}

	// Destructor: Cleans up the semaphores
	~SharedReadDataAccess()
	{
		Cleaner();
	}

	// Static method to remove semaphores
	static void Cleaner()
	{
		std::cout << __FUNCTION__ << "\n";
		bip::named_semaphore::remove(stopSemaphoreName);
		bip::named_semaphore::remove(dataAvailableSemaphoreName);
		bip::named_semaphore::remove(spaceAvailableSemaphoreName);
	}

	// Reads data from the handler if available
	std::span<char> Read() override
	{
		if (dataAvailableSemaphore_->try_wait())
		{
			auto data = handler_->Read();
			spaceAvailableSemaphore_->post();
			return data;
		}
		return {};
	}

	// Stops the read operation
	void Stop() override
	{
		std::cout << __FUNCTION__ << std::endl;
		spaceAvailableSemaphore_->post();
	}

	// Checks if the read operation is running
	bool IsRunning() override
	{
		return !stopSemaphore_->try_wait();
	}
};