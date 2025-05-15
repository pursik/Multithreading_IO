#pragma once
#include <span>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

class IEncrypt
{
public:
	virtual std::span<char> GetIV() = 0;
	virtual void Encrypt(std::span<char> plain, std::span<char> cipher) = 0;
	virtual void Decrypt(std::span<char> cipher, std::span<char> recovered, const CryptoPP::byte* iv) = 0;
};
