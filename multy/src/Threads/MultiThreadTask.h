#pragma once

#include "Config.h"
#include "BufferStateSync.h"
#include "../common/ITask.h"
#include <memory>


// Factory for creating task objects
class MultiThreadTask : public ITask
{
private:
	std::shared_ptr<ITask> reader_ = nullptr;
	std::shared_ptr<ITask> writer_ = nullptr;
	
	BufferStateSync bufferSync_;// a shared running state
	std::atomic<bool> running_ = true;//a shared atomic boolean to control the running state

	MultiThreadTask() = default;
public:
	static std::shared_ptr<ITask> Create(const config::SCommand& command);
	void Run() override;
};
