#include "reader.h"
#include <fstream>
#include <filesystem>

Reader::Reader(IBuffer &buf, IErrorCollectorInterface &errHandler)
	: sharedBuffer_(buf), errorCollector_(errHandler)
{
}

void Reader::Read(std::string_view fileName, size_t dataSize)
{
	try
	{
		std::ifstream file(fileName.data(), std::ios::binary);
		if (!file.is_open())
		{
			if (std::filesystem::exists(fileName.data()))
			{
				errorCollector_.AddError("Failed to open the file " + std::string(fileName));
			}
			else
			{
				errorCollector_.AddError("Input file " + std::string(fileName) + " is not exist.");
			}
		}
		// Attempts to read dataSize bytes into tempBuffer and refill shared biffer
		std::vector<char> tempBuffer(dataSize);
		{
			while (errorCollector_.Empty() && (file.read(tempBuffer.data(), dataSize) /*false If the file reaches the end*/ || file.gcount() > 0 /*actually read amount*/))
			{
				const auto bytesRead = file.gcount(); /*returns the number of bytes that were actually read. <=dataSize */
				if (bytesRead)
				{
					tempBuffer.resize(bytesRead);
				}
				sharedBuffer_.Refill(tempBuffer);
			}
		}
		file.close();
		sharedBuffer_.StopBufferRefilling(); // the buffer refilling is stopped since there is no more data to read from the file
	}
	catch (std::exception &e)
	{
		errorCollector_.AddError(e.what());
	}
	catch (...)
	{
		errorCollector_.AddError("Non-std::exception during reading");
	}
}
