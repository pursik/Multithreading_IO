#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <filesystem>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>

namespace fs = std::filesystem;
namespace po = boost::program_options;
namespace
{
	// Buffer size for reading chunks of the file
	const size_t BUFFER_SIZE = 512;

	// Shared variables for synchronization
	std::vector<char> buffer;
	bool dataAvailable = false;
	bool doneReading = false;
	bool errorReading = false;
	bool errorWriting = false;

	// Mutex and condition variable to synchronize the threads
	std::mutex mtx;
	std::condition_variable cv;
}

void readFromFile(const std::string &sourceFilename)
{
	std::fstream sourceFile(sourceFilename, std::ios::binary | std::ios::in);
	if (!sourceFile)
	{
		std::cerr << "\nError: Could not open source file " << sourceFilename << std::endl;
		errorReading = true;
		cv.notify_one(); // Notify writer thread that reading is crashed
		return;
	}

	while (!sourceFile.eof())
	{
		std::vector<char> tempBuffer(BUFFER_SIZE);

		sourceFile.read(tempBuffer.data(), BUFFER_SIZE);
		std::streamsize bytesRead = sourceFile.gcount(); // Get actual bytes read

		if (bytesRead > 0)
		{
			std::unique_lock<std::mutex> lock(mtx);
			buffer.assign(tempBuffer.begin(), tempBuffer.begin() + bytesRead);
			dataAvailable = true;
			cv.notify_one();
			cv.wait(lock, []
					{ return errorWriting || !dataAvailable; });
		}
	}
	if (!errorWriting)
	{
		std::unique_lock<std::mutex> lock(mtx);
		doneReading = true;
		cv.notify_one(); // Notify writer thread that reading is done
	}
}

void writeToFile(const std::string &targetFilename)
{
	if (fs::exists(targetFilename))
	{
		std::cerr << "\nError: Target file " << targetFilename << " is exist." << std::endl;
		errorWriting = true;
		cv.notify_one();
		return;
	}
	std::ofstream targetFile(targetFilename, std::ios::binary | std::ios::out);
	if (!targetFile)
	{
		std::cerr << "\nError: Could not open target file " << targetFilename << std::endl;
		errorWriting = true;
		cv.notify_one();
		return;
	}

	while (true)
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, []
				{ return errorReading || dataAvailable || doneReading; });

		if (dataAvailable)
		{
			targetFile.write(buffer.data(), buffer.size());
			dataAvailable = false;
			cv.notify_one(); // Notify reader thread to continue reading
		}
		else
		{
			if (errorReading || doneReading)
			{
				break; // Break when reading is done or can't open source file
			}
		}
	}
}



int main(int argc, char *argv[])
{
	try
	{
		po::options_description desc{"Allowed options"};
		desc.add_options()("help,h", "Show help message")("name,n", po::value<std::string>(), "Enter your name");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			std::cout << desc << "\n";
		}

		if (vm.count("name"))
		{
			std::cout << "Hello, " << vm["name"].as<std::string>() << "!\n";
		}

		// Start the reader and writer threads
		// std::thread readerThread(readFromFile, sourceFilename);
		// std::thread writerThread(writeToFile, targetFilename);

		// // Wait for both threads to finish
		// readerThread.join();
		// writerThread.join();
		// if (doneReading)
		// {
		// 	std::cout << "File copying completed successfully." << std::endl;
		// }
	}
	catch (const po::error &ex)
	{
		std::cerr << ex.what() << "\n";
	}

	return 0;
}
