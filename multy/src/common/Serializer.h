#pragma once

#include "../common/DataType.h"
#include "../common/IEncrypt.h"

/**
 * @class CryptSerializer
 * @brief A class that serializes chunk.
 */
class Serializer
{
private:
	DataType type_{ DataType::EMPTY };
	static constexpr uint32_t headerSize = 1 + CryptoPP::AES::BLOCKSIZE + uint32Size;

	std::shared_ptr<IEncrypt> encryptor_; ///< Pointer to an encryption object
	std::vector<char> buffer_; ///< Buffer to store the header and file data.
	std::vector<char> ciphr_;///< Buffer to store the encrypted data
public:
	/**
	* @brief constructor
	* @param encryptor file data
	* @param type data type
	*/
	Serializer(std::shared_ptr<IEncrypt> encryptor, DataType type = DataType::EMPTY) :
		encryptor_(std::move(encryptor)), type_(type) {}

	std::span<char> Pack(std::span<char> data)
	{
		ciphr_.clear();
		buffer_.clear();
		if (data.empty())
		{
			return {};
		}
		ciphr_.resize(data.size());
		encryptor_->Encrypt(data, ciphr_);

		buffer_.reserve(headerSize + ciphr_.size());

		// Add type to the buffer
		buffer_.push_back(static_cast<uint8_t>(type_));

		// Add IV to the buffer
		auto iv_span = encryptor_->GetIV();
		buffer_.insert(buffer_.end(), iv_span.begin(), iv_span.end());

		// Add data size to the buffer (serialize as uint32Size bytes)
		auto dataSize = static_cast<uint32_t>(ciphr_.size());
		auto dataSizePtr = reinterpret_cast<char*>(&dataSize);
		buffer_.insert(buffer_.end(), dataSizePtr, std::next(dataSizePtr, uint32Size));

		// Add the content of the file data
		buffer_.insert(buffer_.end(), ciphr_.begin(), ciphr_.end());
		return { buffer_.data(), buffer_.size() };
	}

	std::span<char> Unpack(std::span<char> data)
	{
		buffer_.clear();
		if (data.empty() || data.size() < headerSize)
		{
			return {};
		}
		type_ = static_cast<DataType>(data[0]);
		auto iv = data.subspan(1, CryptoPP::AES::BLOCKSIZE);

		uint32_t dataSize = 0;
		std::memcpy(&dataSize, data.data() + 1 + CryptoPP::AES::BLOCKSIZE, uint32Size);
		if (dataSize > data.size() - headerSize)
		{
			throw std::invalid_argument("Error: Data size exceeds the available data.");
		}
		buffer_.resize(dataSize);
		encryptor_->Decrypt(
			buffer_,
			data.subspan(headerSize),
			reinterpret_cast<CryptoPP::byte*>(iv.data()));
		return { buffer_.data(), buffer_.size() };
	}

	DataType getType()
	{
		return type_;
	}
};

