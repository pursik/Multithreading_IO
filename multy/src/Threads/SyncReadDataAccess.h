#pragma once

#include <thread>
#include "../common/IReadDataAccess.h"
#include "BufferStateGetter.h"

class SafeReadDataAccess : public IReadDataAccess
{
private:
    std::shared_ptr<IReader> reader_;
    std::shared_ptr<BufferStateGetter> bufferState_;
    std::atomic<bool>& running_; // Atomic flag

public:
    SafeReadDataAccess(std::atomic<bool>& running, std::shared_ptr<IReader> reader, std::shared_ptr<BufferStateGetter> bufferState)
        : reader_(reader), bufferState_(bufferState), running_(running)
    {}

    [[nodiscard]] std::span<char> Read() override
    {
        while (!bufferState_->IsReadingEnabled() && IsRunning()) // Spin until reading is enabled
        {
            std::this_thread::yield(); // Yield to avoid busy-waiting
        }

        if (!running_.load(std::memory_order_acquire)) return {}; // If running is false, terminate the operation

        return reader_->Read(); // Read data from the buffer
    }

    void Stop() override
    {
        running_.store(false, std::memory_order_release); // Set running to false for graceful termination
    }

    [[nodiscard]] bool IsRunning() override
    {
        return running_.load(std::memory_order_acquire);;
    }
};