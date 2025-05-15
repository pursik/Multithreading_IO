#pragma once

class IReader;
class IWriter;
class IEncrypt;
#include <string_view>
#include <memory>
/**
 * @class FileOperationFactory
 * @brief Factory for creating File Reader or File Writer objects.
 */
class FileOperationFactory
{
public:
	/**
  * @brief Creates an FileDataReader instance.
  * @param fileName Name of the file to read from
  * @return Shared pointer to the created FileDataReader instance
  */
	static std::shared_ptr<IReader> CreateFileDataReader(std::string_view fileName);

	/**
  * @brief Creates an FileNameSegmentReader instance.
  * @param fileName Name of the file to read from
  * @param encryptor Shared pointer to an existing IEncryptor instance to be wrapped.
  * @return Shared pointer to the created FileNameSegmentReader instance
  */
	static std::shared_ptr<IReader> CreateFileNameSegmentReader(std::string_view fileName, std::shared_ptr<IEncrypt> encryptor);

	/**
	 * @brief Creates a segmented FileDataSegmentReader instance.
	 *
	 * This method wraps an existing IReader instance to create a segmented reader.
	 * The segmented reader allows reading specific portions of data from the original reader.
	 *
	 * @param reader Shared pointer to an existing IReader instance to be wrapped.
	 * @param encryptor Shared pointer to an existing IEncryptor instance to be wrapped.
	 * @return Shared pointer to the created segmented FileDataReader instance.
	 */
	static std::shared_ptr<IReader> CreateFileDataSegmentReader(std::shared_ptr<IReader> reader, std::shared_ptr<IEncrypt> encryptor);

	/**
  * @brief Creates an FileDataWriter instance.
  * @param fileName Name of the file to write to
  * @param subFolderName Name of the specific folder to write to
  * @return Shared pointer to the created FileDataWriter instance
  */
	static std::shared_ptr<IWriter> CreateFileDataWriter(std::string_view fileName, std::string_view subFolderName = "");
};
