#include "Des.h"
#include "TripleDes.h"

#include <memory.h>


//typedef struct TripleDESData_st
//{
	static uint64_t m_keySchedule[3][16];
	static uint64_t m_iv;
//} TripleDESData_t;

void TripleDesInit()
{
	DesInit();
	memset(&m_keySchedule, 0, sizeof(m_keySchedule));
	memset(&m_iv, 0, sizeof(m_iv));
}

void TripleDesDecryptCBC(uint8_t * plain, const uint8_t * cipher, size_t size)
{
	size_t off;
	uint64_t r;
	uint64_t iv;

	for (off = 0; off < size; off += 8)
	{
		r = BytesToU64(cipher + off);
		iv = r;
		r = InitialPermutation(r);
		r = DecryptInternal(r, m_keySchedule[2]);
		r = EncryptInternal(r, m_keySchedule[1]);
		r = DecryptInternal(r, m_keySchedule[0]);
		r = FinalPermutation(r);
		r = r ^ m_iv;
		m_iv = iv;
		U64ToBytes(plain + off, r);
	}
}

void TripleDesSetKey(const uint8_t * key)
{
	uint64_t k[3];
	memset(m_keySchedule, 0, sizeof(m_keySchedule));
	m_iv = 0;
	k[0] = BytesToU64(key + 0);
	k[1] = BytesToU64(key + 8);
	k[2] = BytesToU64(key + 16);
	KeySchedule(k[0], m_keySchedule[0]);
	KeySchedule(k[1], m_keySchedule[1]);
	KeySchedule(k[2], m_keySchedule[2]);
}

void TripleDesSetIV(const uint8_t * iv)
{
	if (iv)
		m_iv = BytesToU64(iv);
	else
		m_iv = 0;
}
