#pragma once

#include <atomic>
struct BufferStateSync
{
	std::atomic<bool> writingEnabled_ = true; ///< Atomic flag indicating if writing is enabled
	std::atomic<bool> readingEnabled_ = false; ///< Atomic flag indicating if reading is enabled
};

