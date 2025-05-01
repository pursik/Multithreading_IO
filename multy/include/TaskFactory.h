#include <memory>
#include "Config.h"
// Factory for creating task objects
class TaskFactory
{
public:
	TaskFactory() = delete;
	TaskFactory(const TaskFactory&) = delete;
	TaskFactory& operator=(const TaskFactory&) = delete;
	TaskFactory(TaskFactory&&) = delete;
	TaskFactory& operator=(TaskFactory&&) = delete;

	static void Run(const std::optional<config::SCommand>& command);
};