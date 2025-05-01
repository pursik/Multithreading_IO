#pragma once

#include "Config.h"
#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <optional>
#include <vector>

namespace
{
	namespace po = boost::program_options;

	// Command constants
	constexpr auto SOURCE_CMD = "source";
	constexpr auto SOURCE_OPTION = "source,s";
	constexpr auto DEST_CMD = "destination";
	constexpr auto DEST_OPTION = "destination,d";
	constexpr auto MODE_CMD = "mode";
	constexpr auto MODE_OPTION = "mode,m";
	constexpr auto SHARED_MEMORY_CMD = "shared_memory";
	constexpr auto SHARED_MEMORY_OPTION = "shared_memory,r";
	constexpr auto NETWORK_CMD = "network_mode";
	constexpr auto NETWORK_OPTION = "network_mode,n";
	constexpr auto ServerName_CMD = "server_name";
	constexpr auto ServerName_OPTION = "server_name";
	constexpr auto PortName_CMD = "port";
	constexpr auto PortName_OPTION = "port";

	// Mode constants
	constexpr auto mode_multi_thread = "multi_thread";
	constexpr auto mode_multi_process = "multi_process";
	constexpr auto mode_network = "network";

	// Network mode constants
	constexpr auto network_server = "server";
	constexpr auto network_client = "client";
}

/**
 * @class ProgramOptions
 * @brief Stores and parses command line options.
 */
class ProgramOptions
{
public:
	/**
	 * @brief Creates a ProgramOptions instance and parses the command line arguments.
	 * @param argc The number of arguments.
	 * @param argv The argument values.
	 * @return An optional SCommand object if parsing is successful, otherwise std::nullopt.
	 */
	static std::optional<config::SCommand> Create(int argc, const char* argv[])
	{
		return std::shared_ptr<ProgramOptions>(new ProgramOptions)->Parse(argc, argv);
	}

private:
	boost::program_options::options_description description_; ///< Description of the command line options.
	boost::program_options::variables_map commands_; ///< Map of parsed command line options.

	/**
	 * @brief Constructs a ProgramOptions object and initializes the options description.
	 */
	ProgramOptions()
		: description_("Allowed options")
	{
		description_.add_options()
			("help,h", "Display help message.")
			(SOURCE_OPTION, po::value<std::vector<std::string>>(), "Input file name(s).")
			(DEST_OPTION, po::value<std::string>(), "Output file name.")
			(MODE_OPTION, po::value<std::string>()->default_value(mode_multi_thread), "Mode of operation: multi_thread, multi_process, network.")
			(SHARED_MEMORY_OPTION, po::value<std::string>(), "Shared memory name (for multi-process mode).")
			(NETWORK_OPTION, po::value<std::string>(), "Network mode: server or client.")
			(ServerName_OPTION, po::value<std::string>()->default_value("localhost"), "Server name (for network mode).")
			(PortName_OPTION, po::value<std::uint64_t>()->default_value(12345), "Port number (for network mode).");
	}

	/**
	 * @brief Parses the command line arguments.
	 * @param argc The number of arguments.
	 * @param argv The argument values.
	 * @return An optional SCommand object if parsing is successful, otherwise std::nullopt.
	 */
	std::optional<config::SCommand> Parse(int argc, const char* argv[])
	{
		try
		{
			po::store(
				po::command_line_parser(argc, argv)
				.options(description_)
				.style(po::command_line_style::unix_style | po::command_line_style::case_insensitive)
				.run(),
				commands_);
			po::notify(commands_);

			if (argc == 1 || commands_.count("help") > 0)
			{
				std::cout << description_ << std::endl;
				return std::nullopt;
			}

			// Validate mode
			if (commands_.count(MODE_CMD) == 0)
			{
				std::cerr << "Error: Required parameter 'mode' is missing.\n";
				std::cout << description_ << std::endl;
				return std::nullopt;
			}
			const auto modeAsString = commands_[MODE_CMD].as<std::string>();
			const auto mode = ParseMode(modeAsString);
			if (mode == config::Mode::Undefined)
			{
				std::cerr << "Error: Invalid mode '" << modeAsString << "'.\n";
				std::cout << description_ << std::endl;
				return std::nullopt;
			}

			// Validate network mode
			const auto networkAsString = commands_.count(NETWORK_CMD) ? std::optional<std::string>(commands_[NETWORK_CMD].as<std::string>()) : std::nullopt;
			const auto network = ParseNetworkMode(networkAsString);
			if (mode == config::Mode::NETWORK && !network)
			{
				std::cerr << "Error: Required parameter 'network_mode' is missing or invalid.\n";
				std::cout << description_ << std::endl;
				return std::nullopt;
			}

			// Validate shared memory
			const auto shared_memory = commands_.count(SHARED_MEMORY_CMD) ? std::optional<std::string>(commands_[SHARED_MEMORY_CMD].as<std::string>()) : std::nullopt;
			if (mode == config::Mode::MULTI_PROCESS && !shared_memory)
			{
				std::cerr << "Error: Required parameter 'shared_memory' is missing.\n";
				std::cout << description_ << std::endl;
				return std::nullopt;
			}
			
			// Parse other parameters
			const auto dest = commands_.count(DEST_CMD)
				? std::optional<std::string>(commands_[DEST_CMD].as<std::string>())
				: std::nullopt;
			const auto sources = commands_.count(SOURCE_CMD)
				? std::optional<std::vector<std::string>>(commands_[SOURCE_CMD].as<std::vector<std::string>>())
				: std::nullopt;
			const auto serverName = commands_[ServerName_CMD].as<std::string>();
			const auto port = commands_[PortName_CMD].as<std::uint64_t>();

			return config::SCommand{ mode, sources, dest, shared_memory, network, serverName, port };
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << "\n";
			std::cout << description_ << std::endl;
			return std::nullopt;
		}
	}

	/**
	 * @brief Parses the mode string into a config::Mode enum.
	 * @param modeAsString The mode string.
	 * @return The corresponding config::Mode enum value.
	 */
	config::Mode ParseMode(const std::string& modeAsString) const
	{
		if (modeAsString == mode_multi_thread) return config::Mode::MULTI_THREAD;
		if (modeAsString == mode_multi_process) return config::Mode::MULTI_PROCESS;
		if (modeAsString == mode_network) return config::Mode::NETWORK;
		return config::Mode::Undefined;
	}

	/**
	 * @brief Parses the network mode string into a config::NetworkMode enum.
	 * @param networkAsString The network mode string.
	 * @return The corresponding config::NetworkMode enum value, or std::nullopt if invalid.
	 */
	std::optional<config::NetworkMode> ParseNetworkMode(const std::optional<std::string>& networkAsString) const
	{
		if (!networkAsString) return std::nullopt;
		if (*networkAsString == network_server) return config::NetworkMode::SERVER;
		if (*networkAsString == network_client) return config::NetworkMode::CLIENT;
		return std::nullopt;
	}
};
