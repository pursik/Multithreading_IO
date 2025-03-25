#pragma once

#include "ITask.h"
#include "IProcessing.h"
#include "Logger.h"
#include <memory>
#include <boost/interprocess/shared_memory_object.hpp>

class CommandStore;

class MultiProcessTask : public ITask
{
private:
	std::shared_ptr<IProcessing> task_ = nullptr;
	Logger logger_;

	MultiProcessTask() : logger_("MultiProcessTask") {}
public:
	~MultiProcessTask();
	static std::shared_ptr<ITask> Create(CommandStore& commandStore);
	void Run() const override;

	static void Cleaner();
	static void CleaningUp();
};
