#include <stddef.h>
#include <memory.h>
#include <assert.h>

#include "hmac-sha1/hmac/hmac.h"
//#include "Crypto.h"
#include "PBKDF2_HMAC_SHA1.h"

void PBKDF2_HMAC_SHA1(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations, uint8_t* derived_key, size_t dk_len)
{
	assert(salt_len <= 0x20);
	assert(dk_len <= 0x20);

	const size_t h_len = 0x14;
	size_t r;
	size_t l;
	uint8_t t[h_len];
	uint8_t u[h_len];
	uint8_t s[0x24];
	size_t k;
	int j;
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

//		HMAC_SHA1_dd(pw, pw_len, s, salt_len + 4, u);
		size_t u_len = sizeof(u);
		hmac_sha1(pw, pw_len, s, salt_len + 4, u, &u_len);
		memcpy(t, u, sizeof(t));

		for (j = 1; j < iterations; j++)
		{
//			HMAC_SHA1_dd(pw, pw_len, u, sizeof(u), u);
			u_len = sizeof(u);
			hmac_sha1(pw, pw_len, u, sizeof(u), u, &u_len);
			for (n = 0; n < h_len; n++)
				t[n] ^= u[n];
		}

		for (n = 0; n < h_len && (n + k) < dk_len; n++)
			derived_key[n + k] = t[n];
	}
}
