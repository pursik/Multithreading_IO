#pragma once

#include "../common/IWriteDataAccess.h"
#include "BufferStateGetter.h"
#include <thread>

class SafeWriteDataAccess : public IWriteDataAccess
{
private:
	std::shared_ptr<IWriter> writer_;             ///< Shared pointer to the writer instance
	std::shared_ptr<BufferStateGetter> bufferState_; ///< Shared pointer to the buffer state getter
	std::atomic<bool>& running_;                 ///< Reference to an atomic running flag

public:
	/**
	 * @brief Constructor for SafeWriteDataAccess.
	 * @param running Reference to the global `running` flag for thread control
	 * @param writer Shared pointer to the writer instance
	 * @param state Shared pointer to the buffer state getter
	 */
	SafeWriteDataAccess(
		std::atomic<bool>& running,
		std::shared_ptr<IWriter> writer,
		std::shared_ptr<BufferStateGetter> state)
		: writer_(std::move(writer)),
		bufferState_(std::move(state)),
		running_(running)
	{}

	/**
	 * @brief Writes data to the data source in a thread-safe manner.
	 * @param data A span of characters containing the data to write
	 */
	void Write(std::span<char> data) override
	{
		while (!bufferState_->IsWritingEnabled() && IsRunning()) // Wait until writing is enabled or stop signal is received
		{
			std::this_thread::yield(); // Yield to prevent busy-waiting
		}

		if (!running_.load()) return; // Stop writing if the system is no longer running

		if (!data.empty())
		{
			writer_->Write(data); // Perform the write operation
		}
	}

	/**
	 * @brief Stops the writing process.
	 */
	void Stop() override
	{
		while (!bufferState_->IsWritingEnabled()) // Wait until writing is enabled or stop signal is received
		{
			std::this_thread::yield(); // Yield to prevent busy-waiting
		}
		running_.store(false, std::memory_order_release); // Signal all threads to stop running
	}

	/**
	 * @brief Checks if the writing process is running.
	 * @return True if running, false otherwise
	 */
	bool IsRunning() override
	{
		return running_.load(std::memory_order_acquire); // Return the value of the running flag
	}
};
