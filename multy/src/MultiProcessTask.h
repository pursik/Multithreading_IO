#pragma once

#include "ITask.h"
#include "IBuffer.h"
#include "IProcessing.h"
#include <windows.h>
#include <string>
#include <memory>

class CommandStore;

class MultiProcessTask : public ITask
{
private:
	std::shared_ptr<IProcessing> task_ = nullptr;
	std::shared_ptr<IBuffer> buffer_ = nullptr;

	std::string role_;
	HANDLE hMapFile_ = nullptr;
	HANDLE hSemaphore_ = nullptr;
	HANDLE hMutex_ = nullptr;

	MultiProcessTask(std::string_view role) :role_(role) {}
	void CreateTask(CommandStore& commandStore);

public:
	~MultiProcessTask()
	{
		UnmapViewOfFile(buffer_.get());
		CloseHandle(hMapFile_);
		CloseHandle(hSemaphore_);
		CloseHandle(hMutex_);
	}
	static std::shared_ptr<ITask> Create(CommandStore& commandStore);
	void Run() const override;
};
