#include "MultiProcessTask.h"
#include "FileStream.h"
#include "SharedDataAccess.h"
#include "Processing.h"
#include "SharedData.h"
#include "Commands.h"
#include <Windows.h>
#include <iostream>
#include <csignal>

#include <boost/interprocess/file_mapping.hpp>

namespace
{
	constexpr auto mutexServerName = "GlobalMyUniqueServer";
	constexpr auto mutexClientName = "GlobalMyUniqueClient";

	void Terminate()
	{
		std::cout << "Terminate called\n";
		MultiProcessTask::CleaningUp();
		std::abort();
	}
	void SignalHandler(int signal)
	{
		if (signal == SIGINT)
		{
			std::cout << "SIGINT received, cleaning up resources...\n";
			MultiProcessTask::CleaningUp();
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

void MultiProcessTask::CleaningUp()
{
	Cleaner();
	SharedDataAccess::Cleaner();
}

std::shared_ptr<ITask> MultiProcessTask::Create(CommandStore& commandStore)
{
	const auto sharedMemoryName = commandStore.GetStringCommandOption(commands::SHARED_MEMORY_CMD);
	const auto sync = std::make_shared<SharedDataAccess>(
		std::make_shared<SharedDataHandler>(sharedMemoryName));

	const auto inputFilePath = commandStore.GetStringCommandOption(commands::SOURCE_CMD);
	const auto outputFilePath = commandStore.GetStringCommandOption(commands::DEST_CMD);
	if (inputFilePath.empty() || outputFilePath.empty())
	{
		throw std::runtime_error("Error: Required parameters 'source' or 'dest' are missing.\n");
	}
	SetSignalHandler();
	auto instance = std::shared_ptr<MultiProcessTask>(new MultiProcessTask);
	instance->task_ = std::make_shared<Processing>(sync,
		FileStreamFactory::CreateInputFileStream(inputFilePath, DefaultChunkSize),
		FileStreamFactory::CreateOutputFileStream(outputFilePath));
	return instance;
}

void MultiProcessTask::Run() const
{
	try
	{
		//If the mutex already exists, an exception will be thrown.
		std::make_unique<bip::named_mutex>(bip::create_only, mutexClientName);
		std::cout << "Starting as client..." << std::endl;
		task_->Writing();
		return;
	}
	catch (const bip::interprocess_exception& e)
	{
		std::cout << "Client already exists, starting as server...\n";
	}

	try
	{
		//If the mutex already exists, an exception will be thrown.
		std::make_unique<bip::named_mutex>(bip::create_only, mutexServerName);
		task_->Reading();
	}
	catch (const bip::interprocess_exception& e)
	{
		throw std::runtime_error("Server already exist.\n");
	}
}


