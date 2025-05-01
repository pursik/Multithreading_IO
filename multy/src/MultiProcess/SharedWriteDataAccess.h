#pragma once

#include "../common/IWriteDataAccess.h"
#include "../common/Logger.h"
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <mutex>
#include <iostream>
#include <memory>

namespace bip = boost::interprocess;

// Class to handle writing data to shared memory
class SharedWriteDataAccess : public IWriteDataAccess
{
private:
	constexpr static auto mutexName = "GlobalMutex";
	constexpr static auto stopSemaphoreName = "GlobalStopSemaphore";
	constexpr static auto dataAvailableSemaphoreName = "DataAvailableSemaphore";
	constexpr static auto spaceAvailableSemaphoreName = "SpaceAvailableSemaphore";

	std::shared_ptr<IWriter> handler_; // Handler to write data
	std::unique_ptr<bip::named_semaphore> stopSemaphore_; // Semaphore to signal stop
	std::unique_ptr<bip::named_semaphore> dataAvailableSemaphore_; // Semaphore to signal data availability
	std::unique_ptr<bip::named_semaphore> spaceAvailableSemaphore_; // Semaphore to signal space availability
	Logger logger_; // Logger instance

public:
	// Constructor: Initializes the semaphores and handler
	SharedWriteDataAccess(std::shared_ptr<IWriter> handler):
		handler_(std::move(handler)), logger_("SharedWriteDataAccess")
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
	~SharedWriteDataAccess()
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

	// Writes data using the handler
	void Write(std::span<char> data) override
	{
		std::cout << __FUNCTION__ << std::endl;
		spaceAvailableSemaphore_->wait();
		handler_->Write(data);
		dataAvailableSemaphore_->post();
	}

	// Stops the write operation
	void Stop() override
	{
		do
		{
			std::cout << __FUNCTION__ << std::endl;
			stopSemaphore_->post();
		} while (spaceAvailableSemaphore_->try_wait() == false);
	}

	// Checks if the write operation is running
	bool IsRunning() override
	{
		return false;
	}
};
