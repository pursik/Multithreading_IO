#pragma once

#include "Config.h"
#include "../common/ITask.h"
#include "../common/Logger.h"
#include <memory>
#include <boost/interprocess/shared_memory_object.hpp>

class MultiProcessTask : public ITask
{
private:
	std::shared_ptr<ITask> reader_ = nullptr;
	std::shared_ptr<ITask> writer_ = nullptr;
	Logger logger_;

	MultiProcessTask() : logger_("MultiProcessTask") {}
public:
	~MultiProcessTask();
	static std::shared_ptr<ITask> Create(const config::SCommand& command);
	void Run() override;

	static void Cleaner();
};
