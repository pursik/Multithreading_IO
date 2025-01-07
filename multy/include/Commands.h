#ifndef COMMAND_H
#define COMMAND_H

#include <boost/program_options.hpp>
#include <string>
#include <iostream>

namespace commands
{
	constexpr auto SOURCE_CMD = "source";
	constexpr auto SOURCE_OPTION = "source,s";

	constexpr auto DEST_CMD = "destination";
	constexpr auto DEST_OPTION = "destination,d";
}
namespace
{
	namespace po = boost::program_options;
	using namespace commands;
}

class CommandStore
{
public:
	CommandStore()
	{
		description_.add_options()("help,h", "Help message.")(SOURCE_OPTION, po::value<std::string>(), "Input file name.")(DEST_OPTION, po::value<std::string>(), "Output file name.\n");
	}
	virtual bool Parse(int argc, char *argv[])
	{
		po::store(
			po::command_line_parser(argc, argv)
				.options(description_)
				.style(
					boost::program_options::command_line_style::unix_style |
					boost::program_options::command_line_style::case_insensitive)
				.run(),
			commands_);
		po::notify(commands_);

		if (argc == 1 || commands_.count("help") > 0)
		{
			std::cout << description_ << std::endl;
			return false;
		}
		if (commands_.count(SOURCE_CMD) == 0 || commands_.count(DEST_CMD) == 0)
		{
			throw std::runtime_error("Error: Required parameters 'source' or 'dest' are missing.\n");
		}
		return true;
	}

	virtual const boost::program_options::variables_map &GetCommands()
	{
		return commands_;
	}

	virtual std::string GetStringCommandOption(const std::string &command) const
	{
		auto iter = commands_.find(command);
		if (iter != commands_.end())
		{
			return iter->second.as<std::string>();
		}
		return {};
	}
private:
	boost::program_options::options_description description_;
	boost::program_options::variables_map commands_;
};
#endif
