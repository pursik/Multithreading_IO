#pragma once

#include "IReader.h"
#include "IWriter.h"
#include <fstream>
#include <filesystem>

class InputFileStream : public IReader
{
private:
	std::ifstream stream_;
	std::vector<char> chunkBuffer;

	// Private constructor prevents direct construction
	InputFileStream(size_t chunkSize) : chunkBuffer(chunkSize) {}

public:
	static std::shared_ptr<IReader> Create(std::string_view fileName, size_t chunkSize)
	{
		auto instance = std::shared_ptr<InputFileStream>(new InputFileStream(chunkSize));
		instance->stream_.open(fileName.data(), std::ios::binary);
		if (!instance->stream_.is_open())
		{
			std::string error = std::string(fileName);
			if (std::filesystem::exists(fileName.data()))
			{
				throw std::runtime_error(std::string(fileName) + "is failed to open.");
			}
			else
			{
				throw std::runtime_error(std::string(fileName) + " is not exist.");
			}
		}
		return instance;
	}

	std::span<char> Read() override
	{
		if ((stream_.read(chunkBuffer.data(), chunkBuffer.size()) /*false If the file reaches the end*/ || stream_.gcount() > 0 /*actually read amount*/))
		{
			const auto bytesRead = stream_.gcount(); /*returns the number of bytes that were actually read. <=dataSize */
			if (bytesRead)
			{
				chunkBuffer.resize(bytesRead);
			}
			return {chunkBuffer};
		}
		return {};
	}
};

class OutputFileStream : public IWriter
{
private:
	std::ofstream stream_;
	// Private constructor prevents direct construction
	OutputFileStream() = default;

public:
	static std::shared_ptr<IWriter> Create(std::string_view fileName)
	{
		auto instance = std::shared_ptr<OutputFileStream>(new OutputFileStream());
		instance->stream_.open(fileName.data(), std::ios::binary);
		if (!instance->stream_.is_open())
		{
			std::string error = std::string(fileName);
			if (std::filesystem::exists(fileName.data()))
			{
				throw std::runtime_error(std::string(fileName) + "is failed to open.");
			}
			else
			{
				throw std::runtime_error(std::string(fileName) + " is not exist.");
			}
		}
		return instance;
	}

	void Write(std::span<char> data) override
	{
		if (stream_.is_open() && !data.empty())
		{
			stream_.write(data.data(), data.size());
		}
	}
};

// Factory for creating InputFileStream or  OutputFileStream objects
class FileStreamFactory
{
public:
	static std::shared_ptr<IReader> CreateInputFileStream(std::string_view fileName, size_t chunkSize)
	{
		return InputFileStream::Create(fileName, chunkSize);
	}

	static std::shared_ptr<IWriter> CreateOutputFileStream(std::string_view fileName)
	{
		return OutputFileStream::Create(fileName);
	}
};

