/*
 *  Created by jief on 28/08/2018.
 *  Copyright © 2018 jf-luce.
 *  This program is free software; you can redistribute it
 *    and/or modify it under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation.
 */

#include "../darling-dmg/src/DarlingDMGCrypto.h"
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>


int DarlingDMGCrypto_PKCS5_PBKDF2_HMAC_SHA1(const char *pass, int passlen,
                           const unsigned char *salt, int saltlen, int iter,
                           int keylen, unsigned char *out)
{
	int rv = PKCS5_PBKDF2_HMAC_SHA1(pass, passlen, salt, saltlen, iter, keylen, out);
	return rv;
}


void DarlingDMGCrypto_DES_CBC(const unsigned char *key, const unsigned char *iv, unsigned char *out, const unsigned char *in, int inl)
{
//printHexBufAsCDecl(key, 24, "key");
//printHexBufAsCDecl(iv, 32, "iv");
//printHexBufAsCDecl(in, inl, "in");
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	int outl, outl2;

	EVP_CIPHER_CTX_init(ctx);
	EVP_DecryptInit_ex(ctx, EVP_des_ede3_cbc(), NULL, key, iv);
	EVP_DecryptUpdate(ctx, out, &outl, in, inl);
	EVP_DecryptFinal_ex(ctx, out + outl, &outl2);
	outl += outl2;
	EVP_CIPHER_CTX_cleanup(ctx);
	EVP_CIPHER_CTX_free(ctx);
//printHexBufAsCDecl(out, *outl, "expected_result");
}

unsigned char * DarlingDMGCrypto_HMAC(const uint8_t *key, int key_len,
                        const unsigned char *d, size_t n,
                        unsigned char *md, unsigned int *md_len)
{
//printHexBufAsCDecl(key, key_len, "key");
//printHexBufAsCDecl(d, n, "data");
	unsigned char* rv = HMAC(EVP_sha1(), key, key_len, d, n, md, md_len);
//printHexBufAsCDecl(md, *md_len, "expected_result");
	return rv;
}

int DarlingDMGCrypto_aes_key_size()
{
	return sizeof(AES_KEY);
}

int DarlingDMGCrypto_set_aes_decrypt_key(const unsigned char *userKey, const int bits, void **key)
{
//printHexBufAsCDecl(userKey, bits/8, "userKey");
	*key = malloc(sizeof(AES_KEY));
//memset(*key, 0, sizeof(AES_KEY));
	int rv = AES_set_decrypt_key(userKey, bits, (AES_KEY*)*key); // 0 on success
//printHexBufAsCDecl((uint8_t*)*key, sizeof(AES_KEY), "aes_key");
	return rv;
}

/*
 * iv is 16 bytes
 */
void DarlingDMGCrypto_aes_cbc_decrypt(const unsigned char *in, unsigned char *out,
                     size_t length, const void *key,
                     unsigned char *ivec)
{
//printHexBufAsCDecl(in, length, "in");
//printHexBufAsCDecl(ivec, 16, "iv");
	AES_cbc_encrypt((uint8_t*)in, (uint8_t*)out, length, (AES_KEY*)key, ivec, AES_DECRYPT);
//printHexBufAsCDecl(out, length, "expected_result");
}


bool base64Decode(const std::string& input, std::vector<uint8_t>& output)
{
	BIO *b64, *bmem;
	char buffer[input.length()];
	int rd;

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new_mem_buf((void*) input.c_str(), input.length());
	bmem = BIO_push(b64, bmem);
	//BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
	
	rd = BIO_read(bmem, buffer, input.length());
	
	if (rd > 0)
		output.assign(buffer, buffer+rd);

	BIO_free_all(bmem);
	return rd >= 0;
}

