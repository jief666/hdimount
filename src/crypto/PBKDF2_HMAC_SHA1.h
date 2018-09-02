#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void Password_Based_Key_Derivation_Function_2_SHA1(const uint8_t* pw, int pw_len, const uint8_t* salt, int salt_len, int iterations, uint8_t* derived_key, int dk_len);

#ifdef __cplusplus
}
#endif
