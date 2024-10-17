#include "program_options.h"
#include <iostream>

namespace po = boost::program_options;
using namespace program_option;

CommandParseInterface::CommandParseInterface(IErrorCollectorInterface &errorCollector) : errorCollector_(errorCollector)
{
    description_.add_options()("help,h", "Help message.")(SOURCE_OPTION, po::value<std::string>(), "Input file name.")(DEST_OPTION, po::value<std::string>(), "Output file name.\n");
}

std::string CommandParseInterface::ArgumentByOption(const std::string &option)
{
    if (providedOptions_.end() == providedOptions_.find(option) && !providedOptions_[option].defaulted())
    {
        errorCollector_.AddError("Argument --" + std::string(option) + " is missed in command line.");
        return {};
    }
    return providedOptions_[option].as<std::string>();
}

void CommandParseInterface::ParseCommandLine(int argc, char *argv[])
{
    po::store(
        po::command_line_parser(argc, argv)
            .options(description_)
            .style(
                boost::program_options::command_line_style::unix_style |
                boost::program_options::command_line_style::case_insensitive)
            .run(),
        providedOptions_);
    po::notify(providedOptions_);
}

bool CommandParseInterface::HelpMessageRequired(int argc) const
{
    if (argc == 1 || providedOptions_.count("help") > 0U)
    {
        std::cout << description_ << std::endl;
        return true;
    }
    return false;
}
