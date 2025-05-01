#pragma

#include "../common/FileNameTransferTask.h"
#include "../common/FileDataTransferTask.h"
#include "../common/FileTransferTask.h"
#include "../FileStream/FileOperationFactory.h"

#include "../MultiProcess/SharedWriteDataAccess.h"
#include "../MultiProcess/SharedWriteData.h"

#include "../Threads/SyncWriteDataAccess.h"
#include "../Network/TCPClient.h"

// Factory for creating of reading from file and writing to buffer or socket task objects
class ReaderFactory
{
public:
	ReaderFactory() = delete;
	ReaderFactory(const ReaderFactory&) = delete;
	ReaderFactory& operator=(const ReaderFactory&) = delete;
	ReaderFactory(ReaderFactory&&) = delete;
	ReaderFactory& operator=(ReaderFactory&&) = delete;

	template <typename... Args>
	static std::vector<std::shared_ptr<ITask>> CreateTask(const config::SCommand& command, Args&&... args)
	{
		if (command.sources.has_value() == false || command.sources.value().empty())
		{
			throw std::logic_error("Source file name is not set");
		}
		if (command.mode == config::Mode::MULTI_THREAD)
		{
			// Ensure the required arguments are forwarded to SafeWriteDataAccess
			if constexpr (sizeof...(args) == 3)
			{
				auto writer = std::make_shared<SafeWriteDataAccess>(std::forward<Args>(args)...);
				return { std::make_shared<FileDataTransferTask>(writer, FileOperationFactory::CreateFileDataReader(command.sources.value().front())) };
			}
			throw std::logic_error("Mode multi thread invalid arguments");
		}
		if (command.mode == config::Mode::MULTI_PROCESS)
		{
			const auto syncWriter = std::make_shared<SharedWriteDataAccess>(std::make_shared<SharedWriteDataHandler>(command.shared_memory.value()));
			return { std::make_shared<FileDataTransferTask>(syncWriter, FileOperationFactory::CreateFileDataReader(command.sources.value().front())) };
		}
		if (command.mode == config::Mode::NETWORK)
		{
			std::vector<std::shared_ptr<ITask>> tasks;
			const auto portAsString = std::to_string(command.port);

			for (const auto& source : command.sources.value())
			{
				const auto writer = std::make_shared<TCPClient>(command.serverName, portAsString);
				const auto nameSegmentReader = FileOperationFactory::CreateFileNameSegmentReader(source);
				const auto dataSegmentReader = FileOperationFactory::CreateFileDataSegmentReader(FileOperationFactory::CreateFileDataReader(source));

				const auto nameTransferTask = std::make_shared<FileNameTransferTask>(writer, nameSegmentReader);
				const auto dataTransferTask = std::make_shared<FileDataTransferTask>(writer, dataSegmentReader);

				tasks.emplace_back(std::make_shared<FileTransferTask>(nameTransferTask, dataTransferTask));
			}
			return tasks;
		}
		throw std::logic_error("Mode not implemented");
	}
};
