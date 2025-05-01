#pragma once
#include "Config.h" 
#include <span>
#include <array>

// Class to hold data in a fixed-size buffer
class DataBuffer
{
public:
	std::array<char, config::BufferSize> data_; // Use std::array to store the data

	// Constructor: Initializes the shared data with a default size
	DataBuffer() = default; // Default constructor

	// Constructor: Initializes the shared data with the provided data
	DataBuffer(std::span<char> data)
	{
		std::copy(data.begin(), data.end(), data_.begin());
	}
};