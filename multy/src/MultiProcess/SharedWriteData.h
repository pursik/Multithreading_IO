#pragma once

#include "../common/DataBuffer.h"
#include "../common/IWriter.h"
#include "../common/Logger.h"

#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <memory>
#include <string_view>
#include <stdexcept>

namespace bip = boost::interprocess;

// Class to handle writing data to shared memory
class SharedWriteDataHandler : public IWriter
{
	std::shared_ptr<boost::interprocess::shared_memory_object> sharedMemory_;
	std::string_view sharedMemoryName_;
	Logger logger_;
	DataBuffer* data_;
	bip::mapped_region region_;
public:
	// Constructor: Opens or creates shared memory with the given name
	explicit SharedWriteDataHandler(std::string_view sharedMemoryName)
		:sharedMemoryName_(sharedMemoryName)
		, logger_("SharedWriterDataHandler")
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
	virtual ~SharedWriteDataHandler()
	{
		if (sharedMemory_ && sharedMemory_->get_name())
		{
			sharedMemory_->remove(sharedMemoryName_.data());
		}
	}

	// Writes data to shared memory
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
			std::cout << __FUNCTION__ << " region: " << region_.get_size() << std::endl;
			data_ = new (region_.get_address()) DataBuffer(data); // Placement new on the mapped region's address
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
};