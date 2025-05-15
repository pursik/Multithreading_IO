#include "Encryption.h"
#include <cryptopp/osrng.h>
#include <iostream>
#include <string>

Encryption::Encryption()
{
	std::memcpy(_iv.data(), InitIV.data(), CryptoPP::AES::BLOCKSIZE);

	_decryptor.SetKeyWithIV(_key, _key.size(), _iv.data());
	_encryptor.SetKeyWithIV(_key, _key.size(), _iv.data());
}

std::span<char> Encryption::GetIV() 
{
	return { reinterpret_cast<char*>(_iv.data()), _iv.size() };
}

void Encryption::Encrypt(std::span<char> plain, std::span<char> cipher) 
{
	try
	{
		CryptoPP::AutoSeededRandomPool prng;
		prng.GenerateBlock(_iv.data(), CryptoPP::AES::BLOCKSIZE);

		_encryptor.Resynchronize(_iv.data());
		_encryptor.ProcessData(
			reinterpret_cast<CryptoPP::byte*>(cipher.data()),
			reinterpret_cast<const CryptoPP::byte*>(plain.data()),
			plain.size()
		);
		std::cout << "Encryption successful!" << std::endl;
	}
	catch (const CryptoPP::Exception& e)
	{
		throw std::invalid_argument(e.what());
	}
}

void Encryption::Decrypt(std::span<char> recovered, std::span<char> cipher, const CryptoPP::byte* iv)
{
	try
	{
		_decryptor.Resynchronize(iv);
		_decryptor.ProcessData(
			reinterpret_cast<CryptoPP::byte*>(recovered.data()),
			reinterpret_cast<const CryptoPP::byte*>(cipher.data()),
			recovered.size()
		);
		std::cout << "Decryption successful!" << std::endl;
	}
	catch (const CryptoPP::Exception& e)
	{
		throw std::invalid_argument(e.what());
	}
}

