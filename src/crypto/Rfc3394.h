#pragma once

//#include <cstdint>
#include <inttypes.h>
#include <stdlib.h> // size_t
//#include "Aes.h"

int Rfc3394_KeyUnwrap(uint8_t *plain, const uint8_t *crypto, int size, const uint8_t *key, int aes_mode, uint64_t *iv);

void myHMAC_SHA256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t *mac);

