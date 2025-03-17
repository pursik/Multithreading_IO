#include "MultiThreadTask.h"
#include "CircularBuffer.h"
#include "FileStream.h"
#include "SafeDataAccess.h"
#include "Processing.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Commands.h"
#include <thread>

namespace
{
	static constexpr size_t DefaultChunkSize = 4096U;
	static constexpr size_t DefaultChunkAmountInBuffer = 128U;
}
std::shared_ptr<ITask> MultiThreadTask::Create(CommandStore& commandStore)
{
	const auto& inputFilePath = commandStore.GetStringCommandOption(commands::SOURCE_CMD);
	const auto& outputFilePath = commandStore.GetStringCommandOption(commands::DEST_CMD);
	if (inputFilePath.empty() || outputFilePath.empty())
	{
		throw std::runtime_error("Error: Required parameters 'source' or 'dest' are missing.\n");
	}

	auto instance = std::shared_ptr<MultiThreadTask >(new MultiThreadTask());

	const auto bufferState = std::make_shared<BufferState>();

	const auto chunkSize = DefaultChunkSize;
	const auto bufferSize = chunkSize * DefaultChunkAmountInBuffer;
	const auto buffer = std::make_shared<CircularBuffer>(bufferSize, bufferState);

	auto bufferWriter = std::make_shared<BufferWriter>(buffer);
	auto bufferReader = std::make_shared<BufferReader>(buffer);

	auto sync = std::make_shared<SafeDataAccess>(bufferWriter, bufferReader, bufferState);

	auto inputStream = FileStreamFactory::CreateInputFileStream(inputFilePath, chunkSize);
	auto outputStream = FileStreamFactory::CreateOutputFileStream(outputFilePath);

	instance->task_ = std::make_shared<Processing>(sync, inputStream, outputStream);
	return instance;
}

void MultiThreadTask::Run() const
{
	// Create reading thread
	std::thread readerThread(&IProcessing::Reading, task_.get());

	// Create writing thread
	std::thread writerThread(&IProcessing::Writing, task_.get());

	// Wait for reading and writing threads to complete
	readerThread.join();
	writerThread.join();
}
