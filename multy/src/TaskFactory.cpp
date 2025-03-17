#include "MultiThreadTask.h"
#include "MultiProcessTask.h"
#include "TaskFactory.h"
#include "Commands.h"

std::shared_ptr<ITask> TaskFactory::CreateTask(CommandStore& commandStore)
{
	const auto mode = commandStore.GetStringCommandOption(commands::MODE_CMD);
	if (mode == commands::mode_multi_thread)
	{
		return MultiThreadTask::Create(commandStore);
	}
	if (mode == commands::mode_multi_process)
	{
		return MultiProcessTask::Create(commandStore);
	}
	throw std::logic_error("Mode not implemented");
}
