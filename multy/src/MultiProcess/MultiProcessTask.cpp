#include "MultiProcessTask.h"
#include "../Factory/WriterFactory.h"
#include "../Factory/ReaderFactory.h"

#include <iostream>
#include <csignal>

#include <boost/interprocess/sync/named_mutex.hpp>
namespace bip = boost::interprocess;
namespace
{
	constexpr auto mutexServerName = "GlobalMyUniqueServer";
	constexpr auto mutexClientName = "GlobalMyUniqueClient";

	void Terminate()
	{
		std::cout << "Terminate called\n";
		MultiProcessTask::Cleaner();
		std::abort();
	}
	void SignalHandler(int signal)
	{
		if (signal == SIGINT)
		{
			std::cout << "SIGINT received, cleaning up resources...\n";
			MultiProcessTask::Cleaner();
			std::exit(0);
		}
	}
	void SetSignalHandler()
	{
		std::set_terminate(Terminate);
		std::signal(SIGINT, SignalHandler);
	}
}
MultiProcessTask::~MultiProcessTask()
{
	Cleaner();
}

void MultiProcessTask::Cleaner()
{
	std::cout << __FUNCTION__ << "\n";
	bip::named_mutex::remove(mutexClientName);
	bip::named_mutex::remove(mutexServerName);
}

std::shared_ptr<ITask> MultiProcessTask::Create(const config::SCommand& command)
{
	SetSignalHandler();

	auto instance = std::shared_ptr<MultiProcessTask>(new MultiProcessTask);

	instance->reader_ = ReaderFactory::CreateTask(command).front();
	instance->writer_ = WriterFactory::CreateTask(command);
	return instance;
}

void MultiProcessTask::Run()
{
	try
	{
		//If the mutex already exists, an exception will be thrown.
		const auto clientMutex = std::make_unique<bip::named_mutex>(bip::create_only, mutexClientName);
		std::cout << "Starting as client..." << std::endl;
		writer_->Run();
		return;
	}
	catch (const bip::interprocess_exception& e)
	{
		std::cout << "Client already exists, starting as server...\n";
	}

	try
	{
		//If the mutex already exists, an exception will be thrown.
		const auto serverMutex = std::make_unique<bip::named_mutex>(bip::create_only, mutexServerName);
		reader_->Run();
	}
	catch (const bip::interprocess_exception& e)
	{
		throw std::runtime_error("Server already exist.\n");
	}
}


