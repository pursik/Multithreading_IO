#pragma once

#include "../common/IWriter.h"
#include "IBuffer.h"
#include <vector>
#include <memory>

/**
 * @class BufferWriter
 * @brief A class that writes data to a buffer.
 */
class BufferWriter : public IWriter
{
private:
	std::shared_ptr<IBuffer> buffer_; ///< Shared pointer to the buffer

public:
	/**
	 * @brief Constructor for BufferWriter.
	 * @param buffer Shared pointer to the buffer
	 */
	BufferWriter(std::shared_ptr<IBuffer> buffer) : buffer_(buffer) {}

	/**
	 * @brief Writes data to the buffer.
	 * @param data A span of characters containing the data to write
	 */
	void Write(std::span<char> data) override
	{
		for (const char value : data)
		{
			buffer_->Write(value);
		}
	}
};
