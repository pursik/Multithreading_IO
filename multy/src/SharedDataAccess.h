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

class SharedDataAccess : public IDataAccess
{
private:
	std::shared_ptr<IDataAccess> handler_;

	std::unique_ptr<bip::named_semaphore> stopSemaphore_;
	std::unique_ptr<bip::named_semaphore> dataAvailableSemaphore_;
	std::unique_ptr<bip::named_semaphore> spaceAvailableSemaphore_;

	static constexpr auto mutexName = "GlobalMutex";
	static constexpr auto stopSemaphoreName = "GlobalStopSemaphore";
	static constexpr auto dataAvailableSemaphoreName = "DataAvailableSemaphore";
	static constexpr auto spaceAvailableSemaphoreName = "SpaceAvailableSemaphore";
	Logger logger_;

public:
	SharedDataAccess(std::shared_ptr<IDataAccess> handler);

	~SharedDataAccess();

	static void Cleaner();

	void Write(std::span<char> data) override;

	std::span<char> Read() override;

	void Stop() override;

	bool IsRunning() override;
};
