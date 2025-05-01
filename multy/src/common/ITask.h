#pragma once

class ITask
{
public:
	virtual ~ITask() = default;
	virtual void Run() = 0;
};
