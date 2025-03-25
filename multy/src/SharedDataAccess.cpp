#include "SharedDataAccess.h"

SharedDataAccess::SharedDataAccess(std::shared_ptr<IDataAccess> handler) :
	handler_(handler), logger_("SharedDataAccess")
{
	try
	{
		namedMutex_ = std::make_unique<bip::named_mutex>(bip::open_or_create, mutexName);
		stopSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, stopSemaphoreName, 0);
		dataAvailableSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, dataAvailableSemaphoreName, 0);
		spaceAvailableSemaphore_ = std::make_unique<bip::named_semaphore>(bip::open_or_create, spaceAvailableSemaphoreName, 1);
	}
	catch (const bip::interprocess_exception& e)
	{
		throw std::runtime_error("Error: Could not create or open stop condition, mutex, or semaphore.\n");
	}
}

SharedDataAccess::~SharedDataAccess()
{
	Cleaner();
}

void SharedDataAccess::Cleaner()
{
	std::cout << __FUNCTION__ << std::endl;
	bip::named_mutex::remove(mutexName);
	bip::named_semaphore::remove(stopSemaphoreName);
	bip::named_semaphore::remove(dataAvailableSemaphoreName);
	bip::named_semaphore::remove(spaceAvailableSemaphoreName);
}

void SharedDataAccess::Write(std::span<char> data)
{
	std::cout << __FUNCTION__ << std::endl;
	spaceAvailableSemaphore_->wait();
	bip::scoped_lock<bip::named_mutex> lock(*namedMutex_);
	handler_->Write(data);
	dataAvailableSemaphore_->post();
}

std::span<char> SharedDataAccess::Read()
{
	if (dataAvailableSemaphore_->try_wait())
	{
		bip::scoped_lock<bip::named_mutex> lock(*namedMutex_);
		auto data = handler_->Read();
		spaceAvailableSemaphore_->post();
		return data;
	}
	return {};
}

void SharedDataAccess::Stop()
{
	do
	{
		std::cout << __FUNCTION__ << std::endl;
		stopSemaphore_->post();
	} while (spaceAvailableSemaphore_->try_wait() == false);
}

bool SharedDataAccess::IsRunning()
{
	return !stopSemaphore_->try_wait();
}