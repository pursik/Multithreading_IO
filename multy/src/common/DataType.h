#pragma once

static constexpr auto uint32Size = sizeof(uint32_t);

enum class DataType : std::uint8_t
{
	EMPTY = 0U,
	Type_File_Name = 1U,
	Type_File_Data = 2U,
};
