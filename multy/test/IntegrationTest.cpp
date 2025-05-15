#include "TaskFactory.h"
#include "CommandFactory.h"
#include "Encryption.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <array>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/process.hpp>
#include <thread>

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

		const std::filesystem::path GetFullPath() const
		{
			return std::filesystem::absolute(filePath_.data());
		}

	private:
		const std::string_view filePath_;
	};
	constexpr auto DefaultChunkSize = 1024U * 1024U;//1 MB
	constexpr auto DefaultCountOfChunk = 100U;

	void CreateFile(std::string_view fileNmae, size_t chunkSize = DefaultChunkSize, size_t countOfChunk = DefaultCountOfChunk)
	{
		std::ofstream outputFile(fileNmae.data(), std::ios::binary);
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

		for (int i = 0; i < countOfChunk; ++i)
		{
			// Generate  random data
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
}
TEST(PerformanceTest, MultiThread100MbFileTransferTest)
{
	constexpr auto repeat = 10U;
	ScopedFileRemover input(inputFileName.data());
	ScopedFileRemover output(outputFileName.data());

	CreateFile(input.GetPath());

	const char* argv[] = { "progname", "-s", input.GetPath().data(), "-d", output.GetPath().data() };
	int argc = sizeof(argv) / sizeof(char*);

	const auto commandStore = CommandFactory::Create(argc, argv);

	auto startTime = std::chrono::high_resolution_clock::now();
	for (auto i = 0; i < repeat; ++i)
	{
		TaskFactory::Run(commandStore);
	}

	auto endtime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = (endtime - startTime) / repeat;
	std::cout << "elapsed time: " << elapsed.count() << " seconds" << std::endl;
}

TEST(Encryption, EncryptDecryptTest)
{
	std::vector<char> inputData = { '1', '2', '3', '4', '5' };
	std::vector<char> ciphr = { '0', '0', '0', '0', '0' };
	std::vector<char> inFromOut = { '9', '9', '9', '9', '9' };
	Encryption test;
	test.Encrypt(inputData, ciphr);
	test.Decrypt(inFromOut, ciphr, reinterpret_cast<CryptoPP::byte*>(test.GetIV().data()));
	EXPECT_EQ(inputData, inFromOut);
}

TEST(Networking, ServerClientInteractionTest)
{
	static const std::string_view fileName = "input.bin";
	ScopedFileRemover input(fileName.data());
	CreateFile(input.GetPath(), 20, 2);

	boost::process::ipstream is_stream_server;
	boost::process::ipstream is_stream_client;

	boost::process::child serverProcess("FileTransfer.exe -m network  -n server",
		boost::process::std_out > is_stream_server);

	EXPECT_EQ(serverProcess.running(), true);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	boost::process::child clientProcess("FileTransfer.exe -s "+ input.GetFullPath().string() + " -m network -n client",
		boost::process::std_out > is_stream_client);

	EXPECT_EQ(clientProcess.running(), true);
	clientProcess.wait();
	EXPECT_EQ(clientProcess.exit_code(), 0);

	EXPECT_EQ(serverProcess.running(), true);
	serverProcess.terminate();
	serverProcess.wait();

	std::ostringstream client_out;
	client_out << is_stream_client.rdbuf();
	const auto client_out_string = client_out.str();

	std::ostringstream server_out;
	server_out << is_stream_server.rdbuf();
	const auto server_out_string = server_out.str();

	EXPECT_THAT(client_out_string, ::testing::HasSubstr("Connected to server"));
	EXPECT_THAT(client_out_string, ::testing::HasSubstr(fileName));
	EXPECT_THAT(server_out_string, ::testing::HasSubstr("Accepted new connection"));
	EXPECT_THAT(client_out_string, ::testing::HasSubstr("TCPClient::Write"));
	EXPECT_THAT(client_out_string, ::testing::HasSubstr("Encryption successful!"));
	EXPECT_THAT(server_out_string, ::testing::HasSubstr(fileName));
	EXPECT_THAT(server_out_string, ::testing::HasSubstr("Message received from client"));
	EXPECT_THAT(server_out_string, ::testing::HasSubstr("Decryption successful!"));
	EXPECT_THAT(server_out_string, ::testing::HasSubstr("Client disconnected"));

	const auto resultFileName = std::filesystem::current_path() / "client_1" / fileName;
	EXPECT_EQ(std::filesystem::exists(resultFileName), true);
}

