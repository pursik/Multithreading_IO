#include "MultiProcessTask.h"
#include "CircularBuffer.h"
#include "FileStream.h"
#include "GlobalSharedDataAccess.h"
#include "Processing.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "BufferState.h"
#include "Commands.h"

namespace
{
	constexpr auto DefaultChunkSize = 4096U;
	constexpr auto DefaultChunkAmountInBuffer = 128U;
	constexpr auto bufferSize = DefaultChunkSize * DefaultChunkAmountInBuffer;

	constexpr auto sharedMemoryName = "Global\\SharedBuffer";
	constexpr auto semaphore = "Global\\SemaphoreBuffer";

	constexpr auto mutexServerName = "Global\\MyUniqueProcessServerMutex";
	constexpr auto mutexClientName = "Global\\MyUniqueProcessClientMutex";
}

std::shared_ptr<ITask> MultiProcessTask::Create(CommandStore& commandStore)
{
	const auto role = commandStore.GetStringCommandOption(commands::PROCESS_ROLE_CMD);
	if (role != commands::server && role != commands::client)
	{
		throw std::runtime_error("Error: Option role: server or client is missing for Multi-process mode.\n");
	}
	auto instance = std::shared_ptr<MultiProcessTask>(new MultiProcessTask(role));

	instance->hSemaphore_ = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, semaphore);
	if (instance->hSemaphore_ == nullptr)
	{
		instance->hSemaphore_ = CreateSemaphoreA(nullptr, 1, 1, semaphore);
		if (instance->hSemaphore_ == nullptr)
		{
			throw std::runtime_error("Error: Could not create buffer semaphore.\n");
		}
	}

	if (instance->role_ == commands::server)
	{
		instance->hMutex_ = CreateMutexA(NULL, FALSE, mutexServerName);
		if (instance->hMutex_ == NULL)
		{
			throw std::runtime_error("Error: Could not create server mutex.");
		}
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			throw std::runtime_error("Another instance of the server process is already running.");
		}
	}
	else if (instance->role_ == commands::client)
	{
		instance->hMutex_ = CreateMutexA(NULL, FALSE, mutexClientName);
		if (instance->hMutex_ == NULL)
		{
			throw std::runtime_error("Error: Could not create client mutex.");
		}
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			throw std::runtime_error("Another instance of the client process is already running.");
		}
	}
	else
	{
		throw std::runtime_error("Error: Required parameters 'server' or 'client' are missing.\n");
	}

	instance->hMapFile_ = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, sharedMemoryName);
	if (instance->hMapFile_ == nullptr)
	{
		instance->hMapFile_ = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufferSize, sharedMemoryName);
		if (instance->hMapFile_ == nullptr)
		{
			throw std::runtime_error("Error: Could not create shared memory.\n");
		}
	}
	instance->CreateTask(commandStore);
	return instance;
}

void MultiProcessTask::Run() const
{
	if (role_ == commands::server)
	{
		std::cout << "Starting as server..." << std::endl;
		task_->Reading();
	}
	else
	{
		std::cout << "Starting as client..." << std::endl;
		task_->Writing();
	}
}

void MultiProcessTask::CreateTask(CommandStore& commandStore)
{
	const auto bufferState = std::make_shared<InterProcessBufferState>();
	buffer_ = std::make_shared<CircularBuffer>(bufferSize, bufferState, MapViewOfFile(hMapFile_, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize));
	if (buffer_ == nullptr)
	{
		throw std::runtime_error("Client: Could not map view of file.\n");
	}

	const auto sync = std::make_shared<GlobalSharedDataAccess>(
		std::make_shared<BufferWriter>(buffer_),
		std::make_shared<BufferReader>(buffer_),
		bufferState,
		OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, semaphore));

	if (role_ == commands::server)
	{
		const auto& inputFilePath = commandStore.GetStringCommandOption(commands::SOURCE_CMD);
		if(inputFilePath.empty())
		{
			throw std::runtime_error("Error: Required parameters 'source' is missing.\n");
		}
		task_ = std::make_shared<Processing>(sync,
			FileStreamFactory::CreateInputFileStream(inputFilePath, DefaultChunkSize),
			nullptr);
	}
	else
	{
		const auto& outputFilePath = commandStore.GetStringCommandOption(commands::DEST_CMD);
		if(outputFilePath.empty())
		{
			throw std::runtime_error("Error: Required parameters 'dest' is missing.\n");
		}
		task_ = std::make_shared<Processing>(sync,
			nullptr,
			FileStreamFactory::CreateOutputFileStream(outputFilePath));
		bufferState->EnableWriting();
	}
}
