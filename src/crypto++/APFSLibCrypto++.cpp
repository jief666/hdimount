/*
 *  Created by jief on 28/08/2018.
 *  Copyright © 2018 jf-luce.
 *  This program is free software; you can redistribute it
 *    and/or modify it under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation.
 */

#include "../apfs-fuse/ApfsLib/APFSLibCrypto.h"

#include "Crypto.h"
#include "Sha256.h"
#include "AesXts.h"
#include "Crc32++.h"

#include "../Utils.h" // for printHexBufAsCDecl
#include <assert.h>

bool APFSLibCrypto_Rfc3394_KeyUnwrap(uint8_t *plain, const uint8_t *crypto, unsigned int size, const uint8_t *key, int aes_mode, uint64_t *iv)
{
	bool rv = false;
	if ( aes_mode == 128 ) rv = Rfc3394_KeyUnwrap(plain, crypto, size-8, key, AES::AES_128, iv);
	if ( aes_mode == 256 ) rv = Rfc3394_KeyUnwrap(plain, crypto, size-8, key, AES::AES_256, iv);
//printHexBufAsCDecl(plain, size-8, "APFSLibCrypto_Rfc3394_KeyUnwrap");
	return rv;
}

void APFSLibCrypto_PBKDF2_HMAC_SHA256(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations, uint8_t* derived_key, size_t dk_len)
{
//printf("static const char* pass = \"%s\";", pw);
//printHexBufAsCDecl(salt, salt_len, "salt");
//printf("static int iterations = %d;\n", iterations);
	PBKDF2_HMAC_SHA256(pw, pw_len, salt, salt_len, iterations, derived_key, dk_len);
printHexBufAsCDecl(derived_key, dk_len, "APFSLibCrypto_PBKDF2_HMAC_SHA256");
}

void APFSLibCrypto_SHA256(const uint8_t* vek, int vek_len, const uint8_t* uuid, int uuid_len, uint8_t* sha_result)
{
//printHexBufAsCDecl(vek, vek_len, "vek");
//printHexBufAsCDecl(uuid, uuid_len, "uuid");
	SHA256 sha256;
	sha256.Update(vek, vek_len);
	sha256.Update(uuid, uuid_len);
	sha256.Final(sha_result); // 0x20 = 256 bits
printHexBufAsCDecl(sha_result, 0x20, "APFSLibCrypto_SHA256");
}

void APFSLibCrypto_HMAC_SHA256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t *mac)
{
//printHexBufAsCDecl(key, key_len, "key");
//printHexBufAsCDecl(data, data_len, "data");
	HMAC_SHA256(key, key_len, data, data_len, mac);
//printHexBufAsCDecl(mac, 0x20, "APFSLibCrypto_HMAC_SHA256");
}

void APFSLibCrypto_aes_xtx_setkey(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len, AesXts* ctx)
{
	assert(key1_len == 16);
	assert(key2_len == 16);
//printHexBufAsCDecl(key1, key1_len, "key1");
//printHexBufAsCDecl(key2, key2_len, "key2");
	ctx->SetKey(key1, key2);
}

void APFSLibCrypto_aes_xtx_decrypt_internal(const uint8_t* encrypted_text, int len, uint8_t* plain_text, uint64_t uno, AesXts* ctx)
{
//if (encrypted_text[0]==0xD6 && encrypted_text[1]==0x6B && encrypted_text[2]==0x70 && encrypted_text[3]==0x64 ) {
//printf("");
//}
//printHexBufAsCDecl(encrypted_text, len, "encrypted_text");
//printf("uno %ld\n", uno);
	ctx->Decrypt(plain_text, encrypted_text, len, uno);
//printHexBufAsCDecl(plain_text, len, "APFSLibCrypto_aes_xtx_decrypt_internal");
}

static AesXts xts;

void APFSLibCrypto_aes_xtx_setkey(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len)
{
	APFSLibCrypto_aes_xtx_setkey(key1, key1_len, key2, key2_len, &xts);
}

void APFSLibCrypto_aes_xtx_setkey(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len, void **ctx)
{
	*ctx = (void*)new AesXts();
	APFSLibCrypto_aes_xtx_setkey(key1, key1_len, key2, key2_len, (AesXts*)*ctx);
}

void APFSLibCrypto_aes_xtx_decrypt(const uint8_t* encrypted_text, int len, uint8_t* plain_text, uint64_t uno)
{
	APFSLibCrypto_aes_xtx_decrypt_internal(encrypted_text, len, plain_text, uno, &xts);
}

void APFSLibCrypto_aes_xtx_decrypt(const uint8_t* encrypted_text, int len, uint8_t* plain_text, uint64_t uno, void* ctx)
{
	APFSLibCrypto_aes_xtx_decrypt_internal(encrypted_text, len, plain_text, uno, (AesXts*)ctx);
}

static Crc32 g_crc(true, 0x1EDC6F41);

uint32_t APFSLibCrypto_calculate_crc32c(uint32_t crc32c, const unsigned char *buffer, unsigned int length)
{
	g_crc.SetCRC(crc32c);
	g_crc.Calc(buffer, length);
	return g_crc.GetCRC();
}

