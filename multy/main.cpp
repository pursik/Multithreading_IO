#include "TaskFactory.h"
#include "Commands.h"
#include <iostream>
#include <Windows.h>

int main(int argc, const char *argv[])
{
	try
	{
		CommandStore commandStore;
		if (!commandStore.Parse(argc, argv))
		{
			return 0;
		}
		const auto task = TaskFactory::CreateTask(commandStore);
		task->Run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Standard exception: " << e.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cerr << "Unknown exception occurred." << std::endl;
		return -1;
	}
	return 0;
}
