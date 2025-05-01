#pragma once
#include "Config.h"
#include <memory>

// Factory for creating task objects
class CommandFactory
{
public:
	CommandFactory() = delete;
	CommandFactory(const CommandFactory&) = delete;
	CommandFactory& operator=(const CommandFactory&) = delete;
	CommandFactory(CommandFactory&&) = delete;
	CommandFactory& operator=(CommandFactory&&) = delete;

	static std::optional<config::SCommand> Create(int argc, const char* argv[]);
};

