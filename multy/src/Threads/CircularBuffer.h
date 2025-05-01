#pragma once

#include "IBuffer.h"
#include "BufferStateSetter.h"
#include <vector>
#include <memory>
#include <iostream>

/**
 * @class CircularBuffer
 * @brief A circular buffer implementation that supports both in-memory and shared memory modes.
 */
class CircularBuffer : public IBuffer
{
private:
    char* data_; ///< Raw buffer pointer from shared memory
    std::vector<char> dataVec_; ///< Vector used only in in-memory mode
    const size_t capacity_; ///< Capacity of the buffer
    std::shared_ptr<BufferStateSetter> state_; ///< State setter to manage buffer state
    std::atomic<size_t> head_;        // Head index (atomic for lock-free operations)
    std::atomic<size_t> tail_;        // Tail index (atomic for lock-free operations)
public:
    /**
     * @brief Constructor for CircularBuffer.
     * @param size Size of the buffer
     * @param state Shared pointer to the state setter
     * @param mapViewOfFile Pointer to the shared memory view (optional)
     */
    CircularBuffer(size_t size, std::shared_ptr<BufferStateSetter> state, void* mapViewOfFile = nullptr) :
        capacity_(size), state_(state)
    {
        if (mapViewOfFile == nullptr)
        {
            dataVec_.resize(size); // Ensure dataVec_ is properly resized
            data_ = dataVec_.data(); // Update data_ to point to the resized vector
        }
        else
        {
            data_ = static_cast<char*>(mapViewOfFile);
        }
    }

    /**
     * @brief Destructor for CircularBuffer.
     */
    ~CircularBuffer() = default;

    /**
     * @brief Overloaded subscript operator for circular access.
     * @param index Index to access
     * @return Reference to the character at the given index
     */
    char& operator[](size_t index) override
    {
        return data_[index % capacity_]; // Circular access
    }

    /**
     * @brief Gets the size of the data in the buffer.
     * @return Size of the data in the buffer
     */
    size_t GetDataSize() const override
    {
        size_t head = head_.load();
        size_t tail = tail_.load();

        return head <= tail ? tail - head : capacity_ - head + tail; // Handle wrap-around
    }

    /**
     * @brief Writes a character to the buffer.
     * @param item Character to write
     */
    void Write(char item) override
    {
        size_t tail = tail_.load();
        size_t nextTail = (tail + 1) % capacity_;

        if (nextTail == head_.load()) // Buffer full
        {
            state_->DisableWriting();
        }
        else
        {
            data_[tail] = item;
            tail_.store(nextTail);
            state_->EnableReading();
        }
    }

    /**
     * @brief Gets the head index of the buffer.
     * @return Head index of the buffer
     */
    size_t GetHead() const override
    {
        return head_.load();;
    }

    /**
     * @brief Sets the head index of the buffer.
     * @param value New head index value
     */
    void SetHead(size_t value) override
    {
        size_t head = head_.load();
        size_t newHead = (head + value) % capacity_;

        head_.store(newHead);
        state_->EnableWriting();

        if (newHead == tail_.load()) // Buffer empty
        {
            state_->DisableReading();
        }
    }
};
