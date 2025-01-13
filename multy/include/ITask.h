#pragma once

class ITask
{
public:
	virtual ~ITask() = default;
	virtual void Run() const = 0;
};
