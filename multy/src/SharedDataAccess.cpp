#include "SharedDataAccess.h"

SharedDataAccess::SharedDataAccess(std::shared_ptr<IDataAccess> handler) :
	handler_(handler), logger_("SharedDataAccess")
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

SharedDataAccess::~SharedDataAccess()
{
	Cleaner();
}

void SharedDataAccess::Cleaner()
{
	std::cout << __FUNCTION__ << std::endl;
	bip::named_semaphore::remove(stopSemaphoreName);
	bip::named_semaphore::remove(dataAvailableSemaphoreName);
	bip::named_semaphore::remove(spaceAvailableSemaphoreName);
}

void SharedDataAccess::Write(std::span<char> data)
{
	std::cout << __FUNCTION__ << std::endl;
	spaceAvailableSemaphore_->wait();
	handler_->Write(data);
	dataAvailableSemaphore_->post();
}

std::span<char> SharedDataAccess::Read()
{
	if (dataAvailableSemaphore_->try_wait())
	{
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