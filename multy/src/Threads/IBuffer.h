#pragma once
/**
 * @class IBuffer
 * @brief Interface for a buffer that supports reading and writing operations.
 */
class IBuffer
{
public:
    /**
     * @brief Virtual destructor for IBuffer.
     */
    virtual ~IBuffer() = default;

    /**
     * @brief Accesses the element at the specified index.
     * @param index The index of the element to access.
     * @return A reference to the element at the specified index.
     */
    virtual char& operator[](size_t index) = 0;

    /**
     * @brief Writes an item to the buffer.
     * @param item The item to write to the buffer.
     */
    virtual void Write(char item) = 0;

    /**
     * @brief Gets the size of the data in the buffer.
     * @return The size of the data in the buffer.
     */
    virtual size_t GetDataSize() const = 0;

    /**
     * @brief Gets the head index of the buffer.
     * @return The head index of the buffer.
     */
    virtual size_t GetHead() const = 0;

    /**
     * @brief Sets the head index of the buffer.
     * @param value The new head index of the buffer.
     */
    virtual void SetHead(size_t value) = 0;
};
