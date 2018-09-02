/*
 *  Created by jief on 28/08/2018.
 *  Copyright © 2018 jf-luce.
 *  This program is free software; you can redistribute it
 *    and/or modify it under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation.
 */

#include "../../darling-dmg/src/DarlingDMGCrypto.h" // not mandatory, but helps get prototype insconsistency
//#include "aes-rijndael/rijndael-aes.h"
#include "gladman-aes/aes.h"
#include "gladman-sha/sha1.h"
#include "gladman-sha/hmac.h"
#include "PBKDF2_HMAC_SHA1.h"
#include "TripleDes.h"


static void printBuf(uint8_t* buf, size_t count, int line_length = 16)
{
	for (size_t i = 0; i < count;) {
		for (int j = 0; j < line_length; j++)
			printf("%02x ", (uint8_t) (buf[i + j]));
		i += line_length;
		printf("\n");
	}
}




int DarlingDMGCrypto_PKCS5_PBKDF2_HMAC_SHA1(const char *pass, int passlen,
                           const unsigned char *salt, int saltlen, int iter,
                           int keylen, unsigned char *out)
{
//printBuf(salt, saltlen);
	Password_Based_Key_Derivation_Function_2_SHA1((const uint8_t*)pass, (size_t)passlen, (const uint8_t*)salt, (size_t)saltlen, iter, (uint8_t*)out, (size_t)keylen);
//printBuf(out, keylen);
	return 1;
}

// TODO outl unused
void DarlingDMGCrypto_DES_CBC(const unsigned char *key, const unsigned char *iv, unsigned char *out, int *outl, const unsigned char *in, int inl)
{
	TripleDESData_t ctx;
	TripleDesSetKey(key, &ctx);
	TripleDesSetIV(iv, &ctx);
	TripleDesDecryptCBC(out, in, inl, &ctx);
}

unsigned char* DarlingDMGCrypto_HMAC(const uint8_t *key, int key_len,
                        const unsigned char *d, size_t n,
                        unsigned char *md, unsigned int *md_len)
{
	(void)md_len;
	hmac_sha(HMAC_SHA1, key, key_len, d, n, md, *md_len);
	return NULL;
}

int DarlingDMGCrypto_set_aes_decrypt_key(const unsigned char *userKey, const int bits, void **key)
{
	*key = malloc(sizeof(aes_decrypt_ctx));
	if ( bits == 128 ) aes_decrypt_key128(userKey, (aes_decrypt_ctx*)*key);
	if ( bits == 256 ) aes_decrypt_key256(userKey, (aes_decrypt_ctx*)*key);
	return 1;
}

void DarlingDMGCrypto_aes_cbc_decrypt(const unsigned char *in, unsigned char *out,
                     size_t length, const void *key,
                     unsigned char *ivec)
{
   	aes_cbc_decrypt(in, out, length, ivec, (aes_decrypt_ctx*)key);
}


static inline bool is_base64(uint8_t c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

bool base64Decode(const std::string& input, std::vector<uint8_t>& output)
{
	static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
	const int in_len_const = input.size();
	int in_len = input.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	uint8_t char_array_4[4], char_array_3[3];
	
	while (in_len-- && (input[in_] != '='))
	{
		while (!is_base64(input[in_])) // this loop skips non base64 characters
		{
			if (in_ < in_len_const - 1)
			{
				in_++;
				continue;
			}
			else
				break;
		}

		if (in_ == in_len_const - 1)
			break;

		char_array_4[i++] = input[in_]; in_++;
		if (i == 4)
		{
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				output.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			output.push_back(char_array_3[j]);
	}

	return (output.size() > 0);
}
