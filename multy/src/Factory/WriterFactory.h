#pragma

#include "../common/BufferTransferTask.h"
#include "../FileStream/FileOperationFactory.h"

#include "../MultiProcess/SharedReadDataAccess.h"
#include "../MultiProcess/SharedReadData.h"

#include "../Threads/SyncReadDataAccess.h"
#include "../Network/TCPServer.h"

// Factory for creating of reading from buffer or socket and writing to file task object
class WriterFactory
{
public:
	WriterFactory() = delete;
	WriterFactory(const WriterFactory&) = delete;
	WriterFactory& operator=(const WriterFactory&) = delete;
	WriterFactory(WriterFactory&&) = delete;
	WriterFactory& operator=(WriterFactory&&) = delete;

	template <typename... Args>
	static std::shared_ptr<ITask> CreateTask(const config::SCommand& command, Args&&... args)
	{
		if (command.mode == config::Mode::MULTI_THREAD)
		{
			// Ensure the required arguments are forwarded to SafeReadDataAccess
			if constexpr (sizeof...(args) == 3)
			{
				auto reader = std::make_shared<SafeReadDataAccess>(std::forward<Args>(args)...);
				return std::make_shared<BufferTransferTask>(reader, FileOperationFactory::CreateFileDataWriter(command.dest.value()));
			}
			throw std::logic_error("Mode multi thread invalid arguments");
		}
		if (command.mode == config::Mode::MULTI_PROCESS)
		{
			const auto syncReader =
				std::make_shared<SharedReadDataAccess>(std::make_shared<SharedReadDataHandler>(command.shared_memory.value()));
			return std::make_shared<BufferTransferTask>(syncReader, FileOperationFactory::CreateFileDataWriter(command.dest.value()));
		}

		if (command.mode == config::Mode::NETWORK)
		{
			if constexpr (sizeof...(args) == 1)
			{
				return std::make_shared<TCPServer>(std::forward<Args>(args)..., command.port, std::make_shared<Encryption>());
			}
		}

		throw std::logic_error("Mode not implemented");
	}
};