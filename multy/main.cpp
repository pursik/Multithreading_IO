#include "TaskFactory.h"
#include "CommandFactory.h"
#include <iostream>

int main(int argc, const char *argv[])
{
	try
	{
		const auto command = CommandFactory::Create(argc, argv);
		TaskFactory::Run(command);
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
