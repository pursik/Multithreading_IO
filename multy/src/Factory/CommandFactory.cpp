#include "CommandFactory.h"
#include "../Command/ProgramOptions.h"

std::optional<config::SCommand> CommandFactory::Create(int argc, const char* argv[])
{
	return ProgramOptions::Create(argc, argv);
}
