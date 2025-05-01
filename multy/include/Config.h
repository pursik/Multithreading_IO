#pragma once
#include <string>
#include <vector>
#include <optional>

namespace config
{
	/**
	 * @enum Mode
	 * @brief Defines the modes of operation for the commands.
	 *
	 * The mode determines how the commands are executed:
	 * - MULTI_THREAD: Execution using multiple threads.
	 * - MULTI_PROCESS: Execution using multiple processes.
	 * - NETWORK: Execution over a network.
	 */
	enum class Mode
	{
		Undefined, ///< Undefined mode.
		MULTI_THREAD, ///< Multi-threaded mode.
		MULTI_PROCESS, ///< Multi-process mode.
		NETWORK ///< Network-based mode.
	};

	/**
	 * @enum NetworkMode
	 * @brief Defines the network modes for network-based execution.
	 *
	 * The network mode specifies whether the execution is performed
	 * as a server or a client:
	 * - SERVER: Acts as a server in the network.
	 * - CLIENT: Acts as a client in the network.
	 */
	enum class NetworkMode
	{
		SERVER, ///< Server mode.
		CLIENT ///< Client mode.
	};

	/**
	 * @struct SCommand
	 * @brief Represents the configuration for executing commands.
	 *
	 * This structure holds the necessary information for executing commands
	 * in different modes of operation, such as multi-threading, multi-processing,
	 * or network-based execution.
	 */
	struct SCommand
	{
		Mode mode{}; ///< Mode of operation: multi_thread, multi_process, or network.
		std::optional < std::vector<std::string>> sources = std::nullopt; ///< List of source file names to process.
		std::optional < std::string> dest = std::nullopt; ///< Name of the destination file to generate.
		std::optional<std::string> shared_memory = std::nullopt; ///< Shared memory name for multi-process mode.
		std::optional<NetworkMode> network = std::nullopt; ///< Specifies network mode (server or client).
		std::string serverName{};///< Server name or IP address for network communication.
		std::uint64_t port{};///< Port name for network communication.
	};

	constexpr auto DefaultChunkSize = 4096U;
	constexpr auto DefaultChunkAmountInBuffer = 128U;
	constexpr auto BufferSize = DefaultChunkSize * DefaultChunkAmountInBuffer;
}
