#include "NetworkTask.h"
#include "Encryption.h"
#include "../Factory/ReaderFactory.h"
#include "../Factory/WriterFactory.h"

#include <filesystem>
#include <iostream>
#include <csignal>
#include "Windows.h"
namespace
{
	void Terminate()
	{
		std::cout << "Terminate called\n";
		std::abort();
	}
	void SignalHandler(int signal)
	{
		if (signal == SIGINT)
		{
			std::cout << "SIGINT received, cleaning up resources...\n";
			std::exit(0);
		}
	}
	void SetSignalHandler()
	{
		std::set_terminate(Terminate);
		std::signal(SIGINT, SignalHandler);
	}
}

std::shared_ptr<ITask> NetworkTask::Create(const config::SCommand& command)
{
	SetSignalHandler();
	auto instance = std::shared_ptr<NetworkTask>(new NetworkTask);

	if (command.network == config::NetworkMode::SERVER)
	{
		instance->task_list_ = { WriterFactory::CreateTask(command, instance->io_context_) };
	}
	else
	{
		instance->task_list_ = ReaderFactory::CreateTask(command);
	}
	return instance;
}

void NetworkTask::Run()
{
	try
	{
		for (auto& task : task_list_)
		{
			task->Run();
		}
		io_context_.run(); // Run the io_context to process asynchronous operations
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Runtime error: " << e.what() << "\n";
		return;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
		return;
	}
}


