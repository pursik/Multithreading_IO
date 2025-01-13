#pragma once

#include <boost/program_options.hpp>
#include <string>
#include <iostream>

namespace commands
{
	constexpr auto SOURCE_CMD = "source";
	constexpr auto SOURCE_OPTION = "source,s";

	constexpr auto DEST_CMD = "destination";
	constexpr auto DEST_OPTION = "destination,d";

	constexpr auto MODE_CMD = "mode";
	constexpr auto MODE_OPTION = "mode,m";

	constexpr auto mode_multi_thread = "multi_thread";
	constexpr auto mode_multi_process = "multi_process";
	constexpr auto mode_network = "network";
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
		description_.add_options()("help,h", "Help message.")
			(SOURCE_OPTION, po::value<std::string>(), "Input file name.")
			(DEST_OPTION, po::value<std::string>(), "Output file name.\n")
			(MODE_OPTION, po::value<std::string>()->default_value("multi_thread"), "Mode of operation: multi_thread, multi_process, network.");
	}
	virtual bool Parse(int argc, const char *argv[])
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

