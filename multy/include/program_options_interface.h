#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include <string>
namespace program_option
{
	constexpr auto SOURCE_CMD = "source";
	constexpr auto SOURCE_OPTION = "source,s";

	constexpr auto DEST_CMD = "destination";
	constexpr auto DEST_OPTION = "destination,d";
};
class ICommandParseInterface
{
public:
	virtual void ParseCommandLine(int argc, char *argv[]) = 0;
	virtual bool HelpMessageRequired(int argc) const = 0;
	virtual std::string ArgumentByOption(const std::string &option) = 0;
};
#endif
