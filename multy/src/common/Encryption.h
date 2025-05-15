#pragma once
#include "IEncrypt.h"
#include <array>

class Encryption : public IEncrypt
{
	static constexpr std::string_view DefaultCryptoKey = "7D9BB722DA2DC8674E08C3D44AAE976F";
	static constexpr std::string_view InitIV = "37C6D22FADE22B2D924598BEE2455EFC";
	
	CryptoPP::SecByteBlock _key{ reinterpret_cast<const CryptoPP::byte*>(DefaultCryptoKey.data()), DefaultCryptoKey.size() };
	CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption _decryptor;
	CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption _encryptor;
	std::array<CryptoPP::byte, CryptoPP::AES::BLOCKSIZE> _iv;//Ensures a unique key for each block

public:
	Encryption();
	std::span<char> GetIV() override;
	void Encrypt(std::span<char> plain, std::span<char> cipher) override;
	void Decrypt(std::span<char> cipher, std::span<char> recovered, const CryptoPP::byte* iv) override;
};
