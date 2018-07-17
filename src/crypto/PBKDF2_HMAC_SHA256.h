#pragma once

#include <cstdint>

//#include "Aes.h"

void PBKDF2_HMAC_SHA256(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, uint64_t iterations, uint8_t* derived_key, size_t dk_len);
