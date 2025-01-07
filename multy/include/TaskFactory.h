#include "FileProcessing.h"
#include "Buffer.h"
#include "BufferState.h"

class CommandStore;

class ITask
{
public:
	virtual ~ITask() = default;
	virtual void Run() const = 0;
};

class MultiThreadedFileIOTask : public ITask
{
private:
	static constexpr size_t DefaultChunckSize = 256U;
	static constexpr size_t DefaultChunckAmountInBuffer = 4U;

	size_t bufferSize_;
	size_t chunckSize_;
	std::shared_ptr<Processing> task_ = nullptr;
	std::shared_ptr<BaseBuffer> buffer_ = nullptr;
	std::shared_ptr<BufferState> bufferState_ = nullptr;

	constexpr MultiThreadedFileIOTask(size_t chunckSize = DefaultChunckSize, size_t chunckAmountInBuffer = DefaultChunckAmountInBuffer)
		:bufferSize_(chunckSize* chunckAmountInBuffer), chunckSize_(chunckSize){}
public:
	static std::shared_ptr<ITask> Create(CommandStore& commandStore);
	void Run() const override;
};

// Factory for creating task objects
class TaskFactory
{
public:
	static std::shared_ptr<ITask> CreateMultiThreadedFileIOTask(CommandStore& commandStore)
	{
		return MultiThreadedFileIOTask::Create(commandStore);
	}
};