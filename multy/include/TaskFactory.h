#include "ITask.h"
#include <memory>

class CommandStore;
class MultiThreadTask;

// Factory for creating task objects
class TaskFactory
{
public:
	TaskFactory() = delete;
	TaskFactory(const TaskFactory&) = delete;
	TaskFactory& operator=(const TaskFactory&) = delete;
	TaskFactory(TaskFactory&&) = delete;
	TaskFactory& operator=(TaskFactory&&) = delete;

	static std::shared_ptr<ITask> CreateTask(CommandStore& commandStore);
};