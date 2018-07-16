#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

	void TripleDesInit(void);
	void TripleDesDecryptCBC(uint8_t *plain, const uint8_t *cipher, size_t size);

	void TripleDesSetKey(const uint8_t *key);
	void TripleDesSetIV(const uint8_t *iv);

#ifdef __cplusplus
}
#endif
