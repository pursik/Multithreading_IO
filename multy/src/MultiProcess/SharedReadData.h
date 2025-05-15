#pragma once

#include "../common/DataBuffer.h"
#include "../common/IReader.h"
#include "../common/Logger.h"

#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <memory>
#include <string_view>
#include <stdexcept>

namespace bip = boost::interprocess;

// Class to handle reading data from shared memory
class SharedReadDataHandler : public IReader
{
	std::shared_ptr<boost::interprocess::shared_memory_object> sharedMemory_;
	std::string_view sharedMemoryName_;
	Logger logger_;
	DataBuffer* data_;
	bip::mapped_region region_;
public:
	// Constructor: Opens or creates shared memory with the given name
	explicit SharedReadDataHandler(std::string_view sharedMemoryName)
		:sharedMemoryName_(sharedMemoryName)
		, logger_("SharedReadDataHandler")
		, data_(nullptr)
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

	// Destructor: Removes the shared memory
	virtual ~SharedReadDataHandler()
	{
		if (sharedMemory_ && sharedMemory_->get_name())
		{
			sharedMemory_->remove(sharedMemoryName_.data());
		}
	}

	// Reads data from shared memory and returns it as a span
	std::span<char> Read() override
	{
		try
		{
			region_ = bip::mapped_region(*sharedMemory_, bip::read_write);
			if (!region_.get_address())
			{
				throw std::runtime_error("Failed to map shared memory region");
			}
			data_ = reinterpret_cast<DataBuffer*>(region_.get_address());
			std::cout << __FUNCTION__ << " region: " << region_.get_size() << std::endl;
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
};