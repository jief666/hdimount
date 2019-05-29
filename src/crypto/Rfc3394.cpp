#include "Rfc3394.h"

//#include "Sha256.h"
//#include "Util.h"

#include <cstring>
#include <cassert>

#include <iostream>
#include <iomanip>

//#include "../../apfs-fuse/ApfsLib/ApfsEndian.h"
#include "gladman-aes/aes.h"
//#include "aes-rijndael/rijndael-aes.h"

#include "gladman-sha/sha2.h"
#include "gladman-sha/hmac.h"


#ifdef __FreeBSD__
#include <sys/endian.h>
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#include <TargetConditionals.h>

	#if TARGET_RT_LITTLE_ENDIAN
	#define __BYTE_ORDER __LITTLE_ENDIAN

		#define be64toh(x) OSSwapInt64(x)

	#else
	#define __BYTE_ORDER __BIG_ENDIAN

		#define be64toh(x) (x)

	#endif

#elif defined(_WIN32)

	static uint64_t htobe64(uint64_t x) {
		union { uint64_t u64; uint8_t v[8]; } ret;
		ret.v[0] = (uint8_t)(x >> 56);
		ret.v[1] = (uint8_t)(x >> 48);
		ret.v[2] = (uint8_t)(x >> 40);
		ret.v[3] = (uint8_t)(x >> 32);
		ret.v[4] = (uint8_t)(x >> 24);
		ret.v[5] = (uint8_t)(x >> 16);
		ret.v[6] = (uint8_t)(x >> 8);
		ret.v[7] = (uint8_t)x;
		return ret.u64;
	}

	// windows can be only LE
	#define __BYTE_ORDER __LITTLE_ENDIAN // this define is required in HFSCatalogBTree.cpp

	#define be64toh(x)	htobe64(x)

#else
#include <endian.h>
#endif


static const unsigned char default_iv[] = {
  0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
};

constexpr uint64_t rfc_3394_default_iv = 0xA6A6A6A6A6A6A6A6ULL;

int AES_unwrap_key_2(unsigned char *out, const unsigned char *in, int inlen, const uint8_t *key, int aes_mode, uint64_t *iv) {
	aes_decrypt_ctx rijndael_ctx;
	if (aes_mode == 128)
		aes_decrypt_key128((const unsigned char *) key, &rijndael_ctx);
	if (aes_mode == 256)
		aes_decrypt_key256((const unsigned char *) key, &rijndael_ctx);

	unsigned char *A, B[16], *R;
	int i;
	unsigned int j, t;
	inlen -= 8;
	if (inlen & 0x7)
		return -1;
	if (inlen < 8)
		return -1;
	A = B;
	t = 6 * (inlen >> 3);
	memcpy(A, in, 8);
	memcpy(out, in + 8, inlen);
	for (j = 0; j < 6; j++) {
		R = out + inlen - 8;
		for (i = 0; i < inlen; i += 8, t--, R -= 8) {
			A[7] ^= (unsigned char) (t & 0xff);
			if (t > 0xff) {
				A[6] ^= (unsigned char) ((t & 0xff) >> 8);
				A[5] ^= (unsigned char) ((t & 0xff) >> 16);
				A[4] ^= (unsigned char) ((t & 0xff) >> 24);
			}
			memcpy(B + 8, R, 8);
//printHexBufAsCDecl(B, 16);
			aes_decrypt((const unsigned char *) B, (unsigned char *) B, &rijndael_ctx);
//			AES_decrypt(B, B, key);
			memcpy(R, B + 8, 8);
//printHexBufAsCDecl(out, 32, 32);
		}
	}
	uint64_t my_iv = rfc_3394_default_iv;
	if (iv)
		my_iv = *iv;
	if (memcmp(A, &my_iv, 8)) {
		memset(out, 0, inlen);
		return 0;
	}
	return inlen;
}


void Password_Based_Key_Derivation_Function_2_SHA256(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, uint64_t iterations, uint8_t* derived_key, size_t dk_len)
{
	// HMAC_SHA256(pw, key_len, salt, salt_len, mac)

	// l = ceil(dkLen / hLen); // No of blocks
	// r = dkLen - (l - 1) * hLen; // Octets in last block
	// T[k] = F(P, S, c, k + 1); // k = 1 .. iterations

	// F(P, S, c, k) = U_1 ^ U_2 ^ ... U_c
	// U_1 = PRF(P, S || INT(i))
	// U_2 = PRF(P, U_1)
	// U_c = PRF(P, U_{c-1})

	assert(salt_len <= 0x10);
	assert(dk_len <= 0x20);

	constexpr size_t h_len = 0x20;
	size_t r;
	size_t l;
	uint8_t t[h_len];
	uint8_t u[h_len];
	uint8_t s[0x14];
	size_t k;
	uint64_t j;
	uint32_t i;
	size_t n;

	r = dk_len % h_len;
	l = dk_len / h_len;
	if (r > 0) l++;

	for (i = 1, k = 0; k < dk_len; i++, k += h_len)
	{
		// F(P,S,c,i)

		memcpy(s, salt, salt_len);
		s[salt_len + 0] = (i >> 24) & 0xFF;
		s[salt_len + 1] = (i >> 16) & 0xFF;
		s[salt_len + 2] = (i >> 8) & 0xFF;
		s[salt_len + 3] = i & 0xFF;

		//myHMAC_SHA256(pw, pw_len, s, salt_len + 4, u2);
		hmac_sha(HMAC_SHA256, pw, pw_len, s, salt_len + 4, u, sizeof(u));
		memcpy(t, u, sizeof(t));

		for (j = 1; j < iterations; j++)
		{
//			myHMAC_SHA256(pw, pw_len, u, sizeof(u), u);
			hmac_sha(HMAC_SHA256, pw, pw_len, u, sizeof(u), u, sizeof(u));
			for (n = 0; n < h_len; n++)
				t[n] ^= u[n];
		}

		for (n = 0; n < h_len && (n + k) < dk_len; n++)
			derived_key[n + k] = t[n];
	}
}
