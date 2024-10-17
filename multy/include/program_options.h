#ifndef COMMAND_H
#define COMMAND_H

#include "program_options_interface.h"
#include "error_interface.h"
#include <boost/program_options.hpp>

class CommandParseInterface : public ICommandParseInterface
{
public:
	CommandParseInterface(IErrorCollectorInterface &errorCollector);
	void ParseCommandLine(int argc, char *argv[]) override;
	bool HelpMessageRequired(int argc) const override;
	std::string ArgumentByOption(const std::string &option) override;

private:
	boost::program_options::options_description description_;
	boost::program_options::variables_map providedOptions_;
	IErrorCollectorInterface &errorCollector_;
};
#endif
