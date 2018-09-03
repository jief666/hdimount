#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct TripleDESData_st
	{
		uint64_t keySchedule[3][16];
		uint64_t iv;
	} TripleDESData_t;

	void TripleDesSetKey(const uint8_t *key, TripleDESData_t* ctx);
	void TripleDesSetIV(const uint8_t *iv, TripleDESData_t* ctx);
	void TripleDesDecryptCBC(uint8_t *plain, const uint8_t *cipher, size_t size, TripleDESData_t* ctx);

#ifdef __cplusplus
}
#endif
