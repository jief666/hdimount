#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void PBKDF2_HMAC_SHA1(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations, uint8_t* derived_key, size_t dk_len);

#ifdef __cplusplus
}
#endif
