/*
 *  Created by jief on 28/08/2018.
 *  Copyright © 2018 jf-luce.
 *  This program is free software; you can redistribute it
 *    and/or modify it under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation.
 */

#include <assert.h>
#include <string.h> // for memcpy

#include "../../apfs-fuse/ApfsLib/APFSLibCrypto.h"
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/modes.h>

#include "crc32c.h"




//
//static void printBuf(const uint8_t* buf, size_t count, const char* name, int line_length = 8)
//{
//	if ( name ) printf("static const uint8_t %s[] = {\n", name);
//	for (size_t i = 0; i < count;) {
//		printf("\t");
//		for (int j = 0; j < line_length; j++) {
//			if ( i + j < count-1 ) {
//				if ( j < line_length-1 ) {
//					printf("0x%02x, ", (uint8_t) (buf[i + j]));
//				}else{
//					printf("0x%02x,", (uint8_t) (buf[i + j]));
//				}
//			}
//			else if ( i + j < count ) {
//				printf("0x%02x", (uint8_t) (buf[i + j]));
//			}
//		}
//		i += line_length;
//		printf("\n");
//	}
//	if ( name ) printf("};\n");
//}


bool APFSLibCrypto_Rfc3394_KeyUnwrap(uint8_t *plain, const uint8_t *crypto, size_t size, const uint8_t *key, int aes_mode, uint64_t *iv)
{
	AES_KEY aes_key;
	uint64_t myiv = 0xA6A6A6A6A6A6A6A6ULL;

//uint8_t plain2[256];
//Rfc3394_KeyUnwrap(plain2, crypto, size, key, aes_mode, iv);
//uint8_t plain3[1024] = {0};
//uint8_t plain4[1024] = {0};
//int a = AES_set_decrypt_key(key, aes_mode, &aes_key);
//int b = AES_unwrap_key_2(&aes_key, (const uint8_t*)&myiv, plain4, crypto, (unsigned int)size);
//int c = AES_set_decrypt_key(key, aes_mode, &aes_key);
//int d = AES_unwrap_key(&aes_key, (const uint8_t*)iv, plain3, crypto, (unsigned int)size+8);

	AES_set_decrypt_key(key, aes_mode, &aes_key);
	int rv = AES_unwrap_key(&aes_key, (const uint8_t*)&myiv, plain, crypto, (unsigned int)size);
	return rv == size-8;
}

void APFSLibCrypto_PBKDF2_HMAC_SHA256(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations, uint8_t* derived_key, size_t dk_len)
{
//	PBKDF2_HMAC_SHA256(pw, pw_len, salt, salt_len, iterations, derived_key, dk_len);
    PKCS5_PBKDF2_HMAC((const char*)pw, pw_len, salt, salt_len, iterations, EVP_sha256(), dk_len, derived_key);
}

void APFSLibCrypto_SHA256(const uint8_t* vek, int vek_len, const uint8_t* uuid, int uuid_len, uint8_t* sha_result)
{
//	sha256_ctx sha256Ctx;
//	sha256_begin(&sha256Ctx);
//	sha256_hash(vek, vek_len, &sha256Ctx);
//	sha256_hash(uuid, uuid_len, &sha256Ctx);
//	sha_end1(sha_result, &sha256Ctx, 0x20); // 0x20 = 256 bits
	
	SHA256_CTX ctx;
	SHA256_Init(&ctx);
	SHA256_Update(&ctx, vek, vek_len);
	SHA256_Update(&ctx, uuid, uuid_len);
	SHA256_Final(sha_result, &ctx);
	
	
}

void APFSLibCrypto_HMAC_SHA256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t *mac)
{
//	hmac_sha(HMAC_SHA256, key, key_len, data, data_len, mac, 0x20); // 0x20 = 256 bits
	unsigned int md_len = 0x20;
	HMAC(EVP_sha256(), key, key_len, data, data_len, mac, &md_len); // 0x20 = 256 bits

}


/*-----------------------------------------------   AES XTS   -----------------------------------------------*/
void APFSLibCrypto_aes_xtx_setkey(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len, uint8_t* userkey)
{
//	xts_start(0, NULL, key1, key1_len, key2, key2_len, 0, 0, ctx);
//	uint8_t userkey[key1_len+key2_len];
//printBuf(key1, key1_len, "key1");
//printBuf(key2, key2_len, "key2");
	memcpy(userkey, key1, key1_len);
	memcpy(userkey+key1_len, key2, key2_len);
//	AES_set_decrypt_key(userkey, key1_len+key2_len, aes_key);
}

static void APFSLibCrypto_aes_xtx_decrypt_internal(const uint8_t* encrypted_text, int len, uint8_t* plain_text, uint64_t uno, uint8_t* userkey)
{
//	xts_decrypt(encrypted_text, len, plain_text, tweak, ctx);
//printBuf(encrypted_text, len, "encrypted_text");

	uint8_t tweak[0x10];
	for (size_t zz = 0; zz < 0x10; zz++)
	{
		tweak[zz] = uno & 0xFF;
		uno >>= 8;
	}

	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	int outlen, tmplen;
	if (!EVP_DecryptInit_ex(ctx, EVP_aes_128_xts(), NULL, userkey, tweak))
		printf("EVP_aes_128_xts ERROR!! \n");
	if (!EVP_DecryptUpdate(ctx, plain_text, &outlen, encrypted_text, len))
		printf("EVP_aes_128_xts ERROR!! \n");
	if (!EVP_DecryptFinal_ex(ctx, plain_text + outlen, &tmplen))
		printf("EVP_aes_128_xts ERROR!! \n");
	EVP_CIPHER_CTX_free(ctx);
//printBuf(plain_text, len, "expected_result");
}

static uint8_t xts_userkey[32];

void APFSLibCrypto_aes_xtx_setkey(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len)
{
	assert(key1_len+key2_len <= sizeof(xts_userkey));
	APFSLibCrypto_aes_xtx_setkey(key1, key1_len, key2, key2_len, xts_userkey);
}

void APFSLibCrypto_aes_xtx_setkey(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len, void **ctx)
{
	*ctx = malloc(sizeof(key1_len+key2_len));
	APFSLibCrypto_aes_xtx_setkey(key1, key1_len, key2, key2_len, (uint8_t*)*ctx);
}

void APFSLibCrypto_aes_xtx_decrypt(const uint8_t* encrypted_text, int len, uint8_t* plain_text, uint64_t uno)
{
	APFSLibCrypto_aes_xtx_decrypt_internal(encrypted_text, len, plain_text, uno, xts_userkey);
}

void APFSLibCrypto_aes_xtx_decrypt(const uint8_t* encrypted_text, int len, uint8_t* plain_text, uint64_t uno, void* ctx)
{
	APFSLibCrypto_aes_xtx_decrypt_internal(encrypted_text, len, plain_text, uno, (uint8_t*)ctx);
}


/*-----------------------------------------------   Crc32   -----------------------------------------------*/
uint32_t APFSLibCrypto_calculate_crc32c(uint32_t crc32c, const unsigned char *buffer, unsigned int length)
{
	return calculate_crc32c(crc32c, buffer, length);
}

















