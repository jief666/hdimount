#include <stdint.h>

	void DesInit(void);

	uint64_t InitialPermutation(uint64_t v);
	uint64_t FinalPermutation(uint64_t v);
	void KeySchedule(uint64_t key, uint64_t *ks);
	uint64_t EncryptInternal(uint64_t v, const uint64_t *ks);
	uint64_t DecryptInternal(uint64_t v, const uint64_t *ks);
//
//	void PrintBits(uint64_t v, int bits);
//	uint64_t Permute(const uint8_t *box, uint64_t v);
//	uint32_t Function(uint32_t r, uint64_t ks);
//
	uint64_t BytesToU64(const uint8_t *data);
	void U64ToBytes(uint8_t *data, uint64_t val);
//
