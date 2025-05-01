#pragma once

#include "BufferStateSync.h"
#include <iostream>
/**
 * @class BufferState
 * @brief Manages the state of buffer operations, enabling and disabling reading and writing.
 */
class BufferStateSetter
{
public:
	/**
	 * @brief Default constructor for BufferStateSetter.
	 */
	BufferStateSetter(BufferStateSync& sync):sync_(sync)
    {
    }

    /**
    * @brief Enables writing to the buffer.
    */
    inline void EnableWriting() noexcept
    {
        sync_.writingEnabled_.store(true, std::memory_order_release);
    }

    /**
     * @brief Enables reading from the buffer.
     */
    inline void EnableReading() noexcept
    {
        sync_.readingEnabled_.store(true, std::memory_order_release);
    }

    /**
     * @brief Disables writing to the buffer.
     */
    inline void DisableWriting() noexcept
    {
        sync_.writingEnabled_.store(false, std::memory_order_release);
    }

    /**
     * @brief Disables reading from the buffer.
     */
    inline void DisableReading() noexcept
    {
        sync_.readingEnabled_.store(false, std::memory_order_release);
    }

private:
	BufferStateSync& sync_; ///< Reference to the BufferStateSync object
};

