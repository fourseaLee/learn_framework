#include <string.h>
#include <stdio.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/objects.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include "open_ecc.h"
#include <iostream>
#include <crypto++/factory.h>
#include <crypto++/cryptlib.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>
#include <crypto++/osrng.h>
#include <crypto++/keccak.h>
#include <sstream>
using namespace  CryptoPP;

/*ANONYMOUS_NAMESPACE_BEGIN
#if (CRYPTOPP_USE_AES_GENERATOR)
OFB_Mode<AES>::Encryption s_globalRNG;
#else
NonblockingRng s_globalRNG;
#endif
NAMESPACE_END*/

NonblockingRng s_globalRNG;
RandomNumberGenerator & GlobalRNG()
{
	return dynamic_cast<RandomNumberGenerator&>(s_globalRNG);
}


const byte* ConstBytePtr(const std::string& str)
{
    if (str.empty())
        return nullptr;
    return reinterpret_cast<const byte*>(&str[0]);
}


void RandomizedTransfer(BufferedTransformation &source, BufferedTransformation &target, bool finish, const std::string &channel=DEFAULT_CHANNEL)
{
	while (source.MaxRetrievable() > (finish ? 0 : 4096))
	{
		byte buf[4096+64];
		size_t start = GlobalRNG().GenerateWord32(0, 63);
		size_t len = GlobalRNG().GenerateWord32(1, UnsignedMin(4096U, 3*source.MaxRetrievable()/2));
		len = source.Get(buf+start, len);
		target.ChannelPut(channel, buf+start, len);
	}
}

void PutDecodedDatumInto(std::string s1, BufferedTransformation &target)
{
	std::string  s2;
	ByteQueue q;

	while (!s1.empty())
	{
		std::string::size_type pos = s1.find_first_not_of(" ");
		if (pos != std::string::npos)
			s1.erase(0, pos);

		if (s1.empty())
			goto end;

		int repeat = 1;
		if (s1[0] == 'r')
		{
			s1 = s1.erase(0, 1);
			repeat = std::atoi(s1.c_str());
			s1 = s1.substr(s1.find(' ')+1);
		}

		// Convert word32 or word64 to little endian order. Some algorithm test vectors are
		// presented in the format. We probably should have named them word32le and word64le.
		if (s1.length() >= 6 && (s1.substr(0,6) == "word32" || s1.substr(0,6) == "word64"))
		{
			std::istringstream iss(s1.substr(6));
			if (s1.substr(0,6) == "word64")
			{
				word64 value;
				while (iss >> std::skipws >> std::hex >> value)
				{
					value = ConditionalByteReverse(LITTLE_ENDIAN_ORDER, value);
					q.Put(reinterpret_cast<const byte *>(&value), 8);
				}
			}
			else
			{
				word32 value;
				while (iss >> std::skipws >> std::hex >> value)
				{
					value = ConditionalByteReverse(LITTLE_ENDIAN_ORDER, value);
					q.Put(reinterpret_cast<const byte *>(&value), 4);
				}
			}
			goto end;
		}

		s2.clear();
		if (s1[0] == '\"')
		{
			s2 = s1.substr(1, s1.find('\"', 1)-1);
			s1 = s1.substr(s2.length() + 2);
		}
		else if (s1.substr(0, 2) == "0x")
		{
			std::string::size_type n = s1.find(' ');
			StringSource(s1.substr(2, n), true, new HexDecoder(new StringSink(s2)));
			s1 = s1.substr(STDMIN(n, s1.length()));
		}
		else
		{
			std::string::size_type n = s1.find(' ');
			StringSource(s1.substr(0, n), true, new HexDecoder(new StringSink(s2)));
			s1 = s1.substr(STDMIN(n, s1.length()));
		}

		while (repeat--)
		{
			q.Put(ConstBytePtr(s2), s2.size());
			RandomizedTransfer(q, target, false);
		}
	}

end:
	RandomizedTransfer(q, target, true);
}


int main()
{

	RegisterDefaultFactoryFor<HashTransformation, Keccak_256>();
	member_ptr<HashTransformation> hash;
	HashTransformation *pHash = nullptr;
	hash.reset(ObjectFactoryRegistry<HashTransformation>::Registry().CreateObject("Keccak-256"));
	pHash = hash.get();

	// Code coverage
	//(void)hash->AlgorithmName();
//	(void)hash->AlgorithmProvider();
	(void)hash->TagSize();
	(void)hash->DigestSize();
	(void)hash->Restart();
	int digestSize = -1;
	HashVerificationFilter verifierFilter(*pHash, nullptr, HashVerificationFilter::HASH_AT_BEGIN, digestSize);
	PutDecodedDatumInto("a9059cbb2ab09eb219583f4a59a5d0623ade346d962bcd4e46b11da047c9049b", verifierFilter);
	PutDecodedDatumInto("transfer(address,uint256)", verifierFilter);

	verifierFilter.MessageEnd();
    return 0;

}



