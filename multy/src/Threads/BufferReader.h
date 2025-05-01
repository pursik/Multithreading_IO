#pragma once

#include "../common/IReader.h"
#include "IBuffer.h"
#include <iostream>
#include <vector>
#include <memory>

/**
 * @class BufferReader
 * @brief A class that reads data from a buffer.
 */
class BufferReader : public IReader
{
private:
    std::shared_ptr<IBuffer> buffer_; ///< Shared pointer to the buffer
    std::vector<char> chunk_; ///< Vector to store the read chunk of data

public:
    /**
     * @brief Constructor for BufferReader.
     * @param buffer Shared pointer to the buffer
     */
    BufferReader(std::shared_ptr<IBuffer> buffer) : buffer_(buffer) {}

    /**
     * @brief Reads data from the buffer.
     * @return A span of characters containing the read data
     */
    std::span<char> Read() override
    {
        size_t dataSize = buffer_->GetDataSize();
        chunk_.clear();
        chunk_.resize(dataSize);
        const auto head = buffer_->GetHead();
        for (size_t i = 0; i < dataSize; ++i)
        {
            chunk_[i] = (*buffer_)[head + i];
        }
        buffer_->SetHead(dataSize);
        return std::span<char>(chunk_.data(), chunk_.size());
    }
};

