#pragma once

#include "ITask.h"
#include "IBuffer.h"
#include "IProcessing.h"
#include "Logger.h"
#include <windows.h>
#include <string>
#include <memory>
#include <csignal>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

class CommandStore;
namespace bip = boost::interprocess;

class MultiProcessTask : public ITask
{
private:
	std::shared_ptr<IProcessing> task_ = nullptr;
	std::string_view role_;
	std::unique_ptr<bip::shared_memory_object> sharedMemory_;
	std::unique_ptr<bip::mapped_region> region_;
	Logger logger_;

	MultiProcessTask(std::string_view role) :role_(role), logger_("MultiProcessTask") {}
	void CreateTask(CommandStore& commandStore);

public:
	~MultiProcessTask();
	static std::shared_ptr<ITask> Create(CommandStore& commandStore);
	void Run() const override;

	static void Cleaner();
	static void CleaningUp();
};
