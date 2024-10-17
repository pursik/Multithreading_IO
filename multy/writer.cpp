#include "writer.h"
#include <fstream>
#include <filesystem>

Writer::Writer(IBuffer &buf, IErrorCollectorInterface &errorCollector)
	: sharedBuffer_(buf), errorCollector_(errorCollector)
{
}

void Writer::Write(std::string_view fileName)
{
	try
	{
		if (std::filesystem::exists(fileName.data()))
		{
			errorCollector_.AddError("Output file " + std::string(fileName) + " is exist.");
		}
		std::ofstream file(fileName.data(), std::ios::binary);
		if (!file.is_open())
		{
			errorCollector_.AddError("File " + std::string(fileName) + " is not opened for writing.");
		}

		while (errorCollector_.Empty() && (!sharedBuffer_.IsBufferRefillingStopped() || !sharedBuffer_.Empty()))
		{
			const auto chunkOfData = sharedBuffer_.Retrieve();
			if (!chunkOfData.empty())
			{
				file.write(chunkOfData.data(), chunkOfData.size());
			}
		}
		file.close();
	}
	catch (std::exception &e)
	{
		errorCollector_.AddError(e.what());
	}
	catch (...)
	{
		errorCollector_.AddError("Non-std::exception during writing");
	}
}
