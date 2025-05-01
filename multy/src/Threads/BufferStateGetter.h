#pragma once

#include "BufferStateSync.h"
#include <iostream>
/**
 * @class BufferState
 * @brief Manages the state of buffer operations, enabling and disabling reading and writing.
 */
class BufferStateGetter
{
public:
	/**
	 * @brief Default constructor for BufferStateGetter.
	 */
	BufferStateGetter(BufferStateSync& sync) :
		sync_(sync) 
	{
	}

	/**
	 * @brief Checks if writing is enabled.
	 * @return True if writing is enabled, false otherwise.
	 */
	bool IsWritingEnabled()
	{
		return sync_.writingEnabled_.load(std::memory_order_acquire);
	}

	/**
	 * @brief Checks if reading is enabled.
	 * @return True if reading is enabled, false otherwise.
	 */
	bool IsReadingEnabled()
	{
		return sync_.readingEnabled_.load(std::memory_order_acquire);
	}

private:
	BufferStateSync& sync_; ///< Reference to the BufferStateSync object
};

