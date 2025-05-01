#include "TaskFactory.h"

#include "../Threads/MultiThreadTask.h"
#include "../MultiProcess/MultiProcessTask.h"
#include "../Network/NetworkTask.h"


void TaskFactory::Run(const std::optional<config::SCommand>& command)
{
	if(!command)
	{
		throw std::logic_error("Command not provided");
	}
	switch(command->mode)
	{
		case config::Mode::MULTI_THREAD:
			MultiThreadTask::Create(command.value())->Run();
			break;
		case config::Mode::MULTI_PROCESS:
			MultiProcessTask::Create(command.value())->Run();
			break;
		case config::Mode::NETWORK:
			NetworkTask::Create(command.value())->Run();
			break;
		default:
			throw std::logic_error("Unknown command mode");
	 }
}
