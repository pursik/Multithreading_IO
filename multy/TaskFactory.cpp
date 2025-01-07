#include "TaskFactory.h"
#include "BufferProcessing.h"
#include "Commands.h"
#include "FileStream.h"

#include <thread>

void MultiThreadedFileIOTask::Run() const
{
	// Create reading thread
	std::thread readerThread(&Processing::Reading, task_.get());

	// Create writing thread
	std::thread writerThread(&Processing::Writing, task_.get());

	//// Wait for reading and writing threads to complete
	readerThread.join();
	writerThread.join();
}

std::shared_ptr<ITask> MultiThreadedFileIOTask::Create(CommandStore& commandStore)
{
	const auto& inputFilePath = commandStore.GetStringCommandOption(commands::SOURCE_CMD);
	const auto& outputFilePath = commandStore.GetStringCommandOption(commands::DEST_CMD);
	
	auto instance = std::shared_ptr<MultiThreadedFileIOTask>(new MultiThreadedFileIOTask());
	instance->bufferState_ = std::make_shared<BufferState>();
	instance->buffer_ = std::make_shared<CircularBuffer>(instance->bufferSize_, instance->bufferState_);

	auto bufferWriter = std::make_shared<BufferWriter>(instance->buffer_);
	auto bufferReader = std::make_shared<BufferReader>(instance->buffer_);

	auto sync = std::make_shared<SafeDataAccess>(bufferWriter, bufferReader, instance->bufferState_);

	auto inputStream = FileStreamFactory::CreateInputFileStream(inputFilePath, instance->chunckSize_);
	auto outputStream = FileStreamFactory::CreateOutputFileStream(outputFilePath);

	instance->task_ = std::make_shared<Processing>(sync, inputStream, outputStream);
	return instance;
}
