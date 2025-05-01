#pragma once

enum class DataType : std::uint8_t
{
	Type_File_Name = 1U,
	Type_File_Data = 2U,
};


/*#include <iostream>
#include <span>
#include <string>
#include <vector>
#include <cstring> // For std::memcpy

// Define the struct
struct SFileNameSegment {
    uint8_t header = 0x01; // Example header (DataTypeType_File_Name)
    std::string file_name;

    // Helper function to serialize the struct into a contiguous byte array
    std::vector<uint8_t> toByteArray() const {
        std::vector<uint8_t> buffer;

        // Add header to the buffer
        buffer.push_back(header);

        // Add the size of the file_name (as a uint32_t for length prefix)
        uint32_t file_name_size = static_cast<uint32_t>(file_name.size());
        uint8_t size_bytes[sizeof(file_name_size)];
        std::memcpy(size_bytes, &file_name_size, sizeof(file_name_size));
        buffer.insert(buffer.end(), size_bytes, size_bytes + sizeof(file_name_size));

        // Add the content of the file_name
        buffer.insert(buffer.end(), file_name.begin(), file_name.end());

        return buffer; // Return the serialized data
    }
};

int main() {
    // Create an example SFileNameSegment
    SFileNameSegment value;
    value.header = 0x01;
    value.file_name = "example.txt";

    // Serialize the struct into a byte array
    std::vector<uint8_t> serializedData = value.toByteArray();

    // Create a span from the serialized data
    std::span<uint8_t> dataSpan(serializedData);

    // Display the span content
    std::cout << "Serialized Data as Span: ";
    for (uint8_t byte : dataSpan) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    return 0;
}*/