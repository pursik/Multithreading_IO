#pragma once

#include "BufferState.h"
#include "ITask.h"
#include "IBuffer.h"
#include "IProcessing.h"

class CommandStore;

class MultiThreadTask : public ITask
{
private:
	std::shared_ptr<IProcessing> task_ = nullptr;
	MultiThreadTask() = default;
public:
	static std::shared_ptr<ITask> Create(CommandStore& commandStore);
	void Run() const override;
};
