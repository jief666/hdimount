//#include "aes-rijndael/rijndael-aes.h"
#include "gladman-aes/aes.h"
#include <assert.h>

#include "Rfc3394.h"

#include "gladman-sha/sha2.h"
#include "gladman-sha/hmac.h"
//#include "Util.h"

//#include <cstring>
//#include <cassert>
//
//#include <iostream>
//#include <iomanip>

//#include "../../apfs-fuse/ApfsLib/ApfsEndian.h"

union Rfc3394_Unit {
	uint64_t u64[2];
	uint8_t u8[16];
};

uint64_t rfc_3394_default_iv = 0xA6A6A6A6A6A6A6A6ULL;
//
//void PBKDF2_HMAC_SHA256(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, uint64_t iterations, uint8_t* derived_key, size_t dk_len)
//{
//	// HMAC_SHA256(pw, key_len, salt, salt_len, mac)
//
//	// l = ceil(dkLen / hLen); // No of blocks
//	// r = dkLen - (l - 1) * hLen; // Octets in last block
//	// T[k] = F(P, S, c, k + 1); // k = 1 .. iterations
//
//	// F(P, S, c, k) = U_1 ^ U_2 ^ ... U_c
//	// U_1 = PRF(P, S || INT(i))
//	// U_2 = PRF(P, U_1)
//	// U_c = PRF(P, U_{c-1})
//
//	assert(salt_len <= 0x10);
//	assert(dk_len <= 0x20);
//
//	size_t h_len = 0x20;
//	size_t r;
//	size_t l;
//	uint8_t t[h_len];
//	uint8_t u[h_len];
//	uint8_t s[0x14];
//	size_t k;
//	uint64_t j;
//	uint32_t i;
//	size_t n;
//
//	r = dk_len % h_len;
//	l = dk_len / h_len;
//	if (r > 0) l++;
//
//	for (i = 1, k = 0; k < dk_len; i++, k += h_len)
//	{
//		// F(P,S,c,i)
//
//		memcpy(s, salt, salt_len);
//		s[salt_len + 0] = (i >> 24) & 0xFF;
//		s[salt_len + 1] = (i >> 16) & 0xFF;
//		s[salt_len + 2] = (i >> 8) & 0xFF;
//		s[salt_len + 3] = i & 0xFF;
//
////		HMAC_SHA256(pw, pw_len, s, salt_len + 4, u);
//		hmac_sha(HMAC_SHA256, pw, pw_len, s, salt_len + 4, u, sizeof(u));
//		memcpy(t, u, sizeof(t));
//
//		for (j = 1; j < iterations; j++)
//		{
////			HMAC_SHA256(pw, pw_len, u, sizeof(u), u);
//			hmac_sha(HMAC_SHA256, pw, pw_len, u, sizeof(u), u, sizeof(u));
//			for (n = 0; n < h_len; n++)
//				t[n] ^= u[n];
//		}
//
//		for (n = 0; n < h_len && (n + k) < dk_len; n++)
//			derived_key[n + k] = t[n];
//	}
//}
