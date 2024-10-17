#include "buffer.h"
#include "error.h"
#include "reader.h"
#include "writer.h"
#include "program_options.h"

#include <thread>
#include <iostream>

int main(int argc, char* argv[])
{
	try
	{
		constexpr size_t CHUNCK_SIZE = 1024 * 1024;
		ErrorCollector errorCollector;

		CommandParseInterface parser(errorCollector);
		parser.ParseCommandLine(argc, argv);
		if (parser.HelpMessageRequired(argc))
		{
			return 0;
		}
		const auto inputFilePath = parser.ArgumentByOption(program_option::SOURCE_CMD);
		const auto outputFilePath = parser.ArgumentByOption(program_option::DEST_CMD);

		if (errorCollector.Empty())
		{
			Buffer sharedBuffer;

			// Create reading thread
			Reader fileReader(sharedBuffer, errorCollector);
			std::thread readerThread(&Reader::Read, &fileReader, inputFilePath, CHUNCK_SIZE);

			// Create writing thread
			Writer fileWriter(sharedBuffer, errorCollector);
			std::thread writerThread(&Writer::Write, &fileWriter, outputFilePath);

			// Wait for reading and writing threads to complete
			readerThread.join();
			writerThread.join();
		}
		if (!errorCollector.Empty())
		{
			errorCollector.PrintError();
			return -1;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
		return -1;
	}
	catch (...)
	{
		std::cerr << "Non-std::exception during reading";
		return -1;
	}

	std::cout << "The task has succeeded." << std::endl;
	return 0;
}
