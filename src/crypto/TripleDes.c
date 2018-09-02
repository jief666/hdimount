#include "Des.h"
#include "TripleDes.h"

#include <memory.h>



void TripleDesDecryptCBC(uint8_t * plain, const uint8_t * cipher, size_t size, TripleDESData_t* ctx)
{
	size_t off;
	uint64_t r;
	uint64_t iv;

	DesInit();

	for (off = 0; off < size; off += 8)
	{
		r = BytesToU64(cipher + off);
		iv = r;
		r = InitialPermutation(r);
		r = DecryptInternal(r, ctx->keySchedule[2]);
		r = EncryptInternal(r, ctx->keySchedule[1]);
		r = DecryptInternal(r, ctx->keySchedule[0]);
		r = FinalPermutation(r);
		r = r ^ ctx->iv;
		ctx->iv = iv;
		U64ToBytes(plain + off, r);
	}
}

void TripleDesSetKey(const uint8_t * key, TripleDESData_t* ctx)
{
	uint64_t k[3];
	memset(ctx->keySchedule, 0, sizeof(ctx->keySchedule));
	ctx->iv = 0;
	k[0] = BytesToU64(key + 0);
	k[1] = BytesToU64(key + 8);
	k[2] = BytesToU64(key + 16);
	KeySchedule(k[0], ctx->keySchedule[0]);
	KeySchedule(k[1], ctx->keySchedule[1]);
	KeySchedule(k[2], ctx->keySchedule[2]);
}

void TripleDesSetIV(const uint8_t * iv, TripleDESData_t* ctx)
{
	if (iv)
		ctx->iv = BytesToU64(iv);
	else
		ctx->iv = 0;
}
