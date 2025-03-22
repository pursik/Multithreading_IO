#include "MultiProcessTask.h"
#include "CircularBuffer.h"
#include "FileStream.h"
#include "GlobalSharedDataAccess.h"
#include "Processing.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "BufferState.h"
#include "Commands.h"

#include <boost/interprocess/file_mapping.hpp>
namespace
{
	constexpr auto DefaultChunkSize = 4096U;
	constexpr auto DefaultChunkAmountInBuffer = 128U;
	constexpr auto bufferSize = DefaultChunkSize * DefaultChunkAmountInBuffer;

	constexpr auto sharedMemoryName = "GlobalSharedBuffer";
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
	std::cout << "~MultiProcessTask..." << std::endl;
	Cleaner();
}

void MultiProcessTask::Cleaner()
{
	std::cout << __FUNCTION__ << "\n";
	bip::shared_memory_object::remove(sharedMemoryName);
	bip::named_mutex::remove(mutexServerName);
	bip::named_mutex::remove(mutexClientName);
}

void MultiProcessTask::CleaningUp()
{
	Cleaner();
	GlobalSharedDataAccess::Cleaner();
	InterProcessBufferState::Cleaner();
}

std::shared_ptr<ITask> MultiProcessTask::Create(CommandStore& commandStore)
{
	SetSignalHandler();
	auto instance = std::shared_ptr<MultiProcessTask>(new MultiProcessTask{ commandStore.GetStringCommandOption(commands::PROCESS_ROLE_CMD) });
	instance->CreateTask(commandStore);
	return instance;
}

void MultiProcessTask::Run() const
{
	try
	{
		if (role_ == commands::server)
		{
			std::make_unique<bip::named_mutex>(bip::create_only, mutexServerName);//If the mutex already exists, an exception will be thrown.
			std::cout << "Starting as server..." << std::endl;
			task_->Reading();
		}
		else
		{
			std::make_unique<bip::named_mutex>(bip::create_only, mutexClientName);//If the mutex already exists, an exception will be thrown.
			std::cout << "Starting as client..." << std::endl;
			task_->Writing();
		}
	}
	catch (const bip::interprocess_exception& e)
	{
		throw std::runtime_error("Error: " + std::string(role_) + " already exists.\n");
	}
}

void MultiProcessTask::CreateTask(CommandStore& commandStore)
{
	try
	{
		sharedMemory_ = std::make_unique<bip::shared_memory_object>(bip::open_or_create, sharedMemoryName, bip::read_write);
		sharedMemory_->truncate(bufferSize);
		region_ = std::make_unique<bip::mapped_region>(*sharedMemory_, bip::read_write);

		const auto bufferState = std::make_shared<InterProcessBufferState>();

		auto buffer = std::make_shared<CircularBuffer>(bufferSize, bufferState, region_->get_address());
		if (buffer == nullptr)
		{
			throw std::runtime_error("Client: Could not map view of file.\n");
		}
		const auto sync = std::make_shared<GlobalSharedDataAccess>(
			std::make_shared<BufferWriter>(buffer),
			std::make_shared<BufferReader>(buffer),
			bufferState);

		const auto inputFilePath = commandStore.GetStringCommandOption(commands::SOURCE_CMD);
		const auto outputFilePath = commandStore.GetStringCommandOption(commands::DEST_CMD);
		if (inputFilePath.empty())
		{
			throw std::runtime_error("Error: Required parameters 'source' is missing.\n");
		}
		if (outputFilePath.empty())
		{
			throw std::runtime_error("Error: Required parameters 'dest' is missing.\n");
		}
		task_ = std::make_shared<Processing>(sync,
			FileStreamFactory::CreateInputFileStream(inputFilePath, DefaultChunkSize),
			FileStreamFactory::CreateOutputFileStream(outputFilePath));
		if (role_ == commands::client)
		{
			bufferState->EnableWriting();
		}

	}
	catch (const std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (const bip::interprocess_exception& e)
	{
		throw std::runtime_error("Error: Could not create or open shared memory.\n");
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error(e.what());
	}
}

