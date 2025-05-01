#include "MultiThreadTask.h"
#include "BufferStateGetter.h"
#include "BufferStateSetter.h"
#include "CircularBuffer.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "SyncReadDataAccess.h"
#include "../Factory/WriterFactory.h"
#include "../Factory/ReaderFactory.h"

#include <thread>

std::shared_ptr<ITask> MultiThreadTask::Create(const config::SCommand& command)
{
	auto instance = std::shared_ptr<MultiThreadTask >(new MultiThreadTask());

	// Create a circular buffer with the specified size
	const auto bufferStateSetter = std::make_shared<BufferStateSetter>(instance->bufferSync_);
	const auto bufferStateGetter = std::make_shared<BufferStateGetter>(instance->bufferSync_);
	const auto buffer = std::make_shared<CircularBuffer>(config::BufferSize, bufferStateSetter);

	instance->reader_ = ReaderFactory::CreateTask(command, instance->running_, std::make_shared<BufferWriter>(buffer), bufferStateGetter).front();
	instance->writer_ = WriterFactory::CreateTask(command, instance->running_, std::make_shared<BufferReader>(buffer), bufferStateGetter);

	return instance;
}

void MultiThreadTask::Run()
{
	// Create reading thread
	std::thread readerThread(&ITask::Run, reader_.get());

	// Create writing thread
	std::thread writerThread(&ITask::Run, writer_.get());

	// Wait for reading and writing threads to complete
	readerThread.join();
	writerThread.join();
}
