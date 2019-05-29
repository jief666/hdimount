#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#ifdef _MSC_VER
#include <malloc.h> // for alloca
#else
#include <alloca.h>
#endif

//#include "hmac-sha1/hmac/hmac.h"
#include "gladman-sha/sha1.h"
#include "gladman-sha/hmac.h"
//#include "Crypto.h"
#include "PBKDF2_HMAC_SHA1.h"

void Password_Based_Key_Derivation_Function_2_SHA1(const uint8_t* pw, int pw_len, const uint8_t* salt, int salt_len, int iterations, uint8_t* derived_key, int dk_len)
{
	assert(salt_len > 0);
	assert(salt_len <= 0x20);
	assert(dk_len > 0);
	assert(dk_len <= 0x20);

	const size_t h_len = 0x14;
	size_t r;
	size_t l;
	uint8_t* t = (uint8_t*)alloca(h_len);
	uint8_t* u = (uint8_t*)alloca(h_len);
	uint8_t s[0x24];
	size_t k;
	int j;
	uint32_t i;
	size_t n;

	r = dk_len % h_len;
	l = dk_len / h_len;
	if (r > 0) l++;

	for (i = 1, k = 0; k < (unsigned int)dk_len; i++, k += h_len)
	{
		// F(P,S,c,i)

		memcpy(s, salt, salt_len);
		s[salt_len + 0] = (i >> 24) & 0xFF;
		s[salt_len + 1] = (i >> 16) & 0xFF;
		s[salt_len + 2] = (i >> 8) & 0xFF;
		s[salt_len + 3] = i & 0xFF;

//		HMAC_SHA1_dd(pw, pw_len, s, salt_len + 4, u);
//		size_t u_len = h_len;
//		hmac_sha1(pw, pw_len, s, salt_len + 4, u, &u_len);
		hmac_sha(HMAC_SHA1, pw, pw_len, s, salt_len + 4, u, h_len);
		memcpy(t, u, h_len);

		for (j = 1; j < iterations; j++)
		{
//			HMAC_SHA1_dd(pw, pw_len, u, sizeof(u), u);
//			u_len = h_len;
//			hmac_sha1(pw, pw_len, u, sizeof(u), u, &u_len);
			hmac_sha(HMAC_SHA1, pw, pw_len, u, h_len, u, h_len);
			for (n = 0; n < h_len; n++)
				t[n] ^= u[n];
		}

		for (n = 0; n < h_len && (n + k) < (unsigned int)dk_len; n++)
			derived_key[n + k] = t[n];
	}
}
