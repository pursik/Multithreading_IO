
#include "IDataAccess.h"
#include "Logger.h"
#include <cstring>
#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <span>
#include <memory>
#include <string_view>
#include <array>
#include <stdexcept>
#include <windows.h>

namespace bip = boost::interprocess;
constexpr auto DefaultChunkSize = 4096U;
constexpr auto DefaultChunkAmountInBuffer = 128U;
constexpr auto bufferSize = DefaultChunkSize * DefaultChunkAmountInBuffer;

class SharedData
{
public:
	std::array<char, bufferSize> data_; // Use std::array to store the data

	SharedData(std::span<char> data)
	{
		std::copy(data.begin(), data.end(), data_.begin());
	}
};

// --- Shared Data Class ---
class SharedDataHandler : public IDataAccess
{
	std::shared_ptr<boost::interprocess::shared_memory_object> sharedMemory_;
	std::string_view sharedMemoryName_;
	Logger logger_;
	SharedData* data_;
	bip::mapped_region region_;
public:
	// Constructor: Writes data to shared memory
	explicit SharedDataHandler(std::string_view sharedMemoryName)
		:sharedMemoryName_(sharedMemoryName)
		, logger_("SharedDataHandler")
	{
		try
		{
			sharedMemory_ =
				std::make_shared<bip::shared_memory_object>(bip::open_or_create, sharedMemoryName_.data(), bip::read_write);
		}
		catch (const bip::interprocess_exception& e)
		{
			throw std::runtime_error("Error: Could not create or open shared memory.\n");
		}
	}

	// Destructor: Logs memory release
	virtual ~SharedDataHandler()
	{
		if (sharedMemory_ && sharedMemory_->get_name())
		{
			sharedMemory_->remove(sharedMemoryName_.data());
		}
	}

	void Write(std::span<char> data) override
	{
		try
		{
			sharedMemory_->truncate(data.size());
			region_ = bip::mapped_region(*sharedMemory_, bip::read_write);
			if (!region_.get_address())
			{
				throw std::runtime_error("Failed to map shared memory region");
			}
			std::cerr << __FUNCTION__ << " region: " << region_.get_size() << std::endl;
			data_ = new (region_.get_address()) SharedData(data); // Placement new on the mapped region's address
		}
		catch (const std::exception& e)
		{
			std::cerr << "Write error: " << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "Write error: Unknown exception" << std::endl;
		}
	}

	std::span<char> Read() override
	{
		try
		{
			region_ = bip::mapped_region(*sharedMemory_, bip::read_write);
			if (!region_.get_address())
			{
				throw std::runtime_error("Failed to map shared memory region");
			}
			data_ = reinterpret_cast<SharedData*>(region_.get_address());
			std::cerr << __FUNCTION__ << " region: " << region_.get_size() << std::endl;
			if (region_.get_size())
			{
				return { data_->data_.data(), region_.get_size() };
			}
			return {};
		}
		catch (const std::exception& e)
		{
			std::cerr << "Read error: " << e.what() << std::endl;
			return {};
		}
	}

	void Stop() override {}

	bool IsRunning() override { return true; }
};