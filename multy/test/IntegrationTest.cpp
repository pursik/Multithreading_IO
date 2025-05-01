#include "TaskFactory.h"
#include "CommandFactory.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <gtest/gtest.h>

namespace
{
	static const std::string_view inputFileName = "input_100MB.bin";
	const std::string_view outputFileName = "output_100MB.bin";

	/**
	  * Remove a file when object goes out of scope.
	  */
	class ScopedFileRemover
	{
	public:
		explicit ScopedFileRemover(std::string_view filePath) :
			filePath_(filePath)
		{
			std::error_code errorCode{};
			if (std::filesystem::exists(filePath_.data(), errorCode))
			{
				std::filesystem::remove_all(filePath_.data(), errorCode);
			}
		}
		ScopedFileRemover(const ScopedFileRemover&) = delete;
		ScopedFileRemover& operator=(const ScopedFileRemover&) = delete;
		ScopedFileRemover(ScopedFileRemover&&) = delete;
		ScopedFileRemover& operator=(ScopedFileRemover&&) = delete;
		~ScopedFileRemover() noexcept = default;

		const std::string_view GetPath() const
		{
			return filePath_;
		}

	private:
		const std::string_view filePath_;
	};

	void Create100mbFile(std::string_view fileNmae)
	{
		std::ofstream outputFile(fileNmae.data(), std::ios::binary);
		constexpr auto chunkSize = 1024U * 1024U;//1 MB
		constexpr auto countOfChunk = 100U;

		if (!outputFile)
		{
			std::cerr << "Failed to create file." << std::endl;
			return;
		}
		std::vector<char> chunk(chunkSize);

		// Simulate binary data
		std::uniform_int_distribution<uint16_t> dist(0, 255);//Creates a distribution that produces random integers uniformly distributed over the range [0, 255].
		std::random_device random;//is used to initialize the internal state of std::mt19937
		std::mt19937 engine(random()/*Generates a random integer */);//generates a long sequence of pseudo-random numbers. 

		// Generate and write 100 pieces of 1 MB random data
		for (int i = 0; i < countOfChunk; ++i)
		{
			// Generate 1MB of random data
			std::generate(chunk.begin(), chunk.end(), [&]() { return static_cast<char>(dist(engine)); });

			// Write the data to file
			outputFile.write(chunk.data(), chunk.size());
			if (!outputFile)
			{
				std::cerr << "Failed to write to file." << std::endl;
				return;
			}
		}
	}

	TEST(PerformanceTest, MultiThread100MbFileTransferTest)
	{
		constexpr auto repeat = 10U;
		ScopedFileRemover input(inputFileName.data());
		ScopedFileRemover output(outputFileName.data());

		Create100mbFile(input.GetPath());

		const char* argv[] = { "progname", "-s", input.GetPath().data(), "-d", output.GetPath().data() };
		int argc = sizeof(argv) / sizeof(char*);

		const auto commandStore = CommandFactory::Create(argc, argv);

		auto startTime = std::chrono::high_resolution_clock::now();
		for (auto i = 0; i < repeat; ++i)
		{
			TaskFactory::Run(commandStore);
		}

		auto endtime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = (endtime - startTime)/ repeat;
		std::cout << "elapsed time: " << elapsed.count() << " seconds" << std::endl;
	}
}
