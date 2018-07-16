/*
This file is part of apfs-fuse, a read-only implementation of APFS
(Apple File System) for FUSE.
Copyright (C) 2017 Simon Gander

Apfs-fuse is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Apfs-fuse is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with apfs-fuse.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <memory> // for shared_ptr

#include "../darling-dmg/src/Reader.h"

#define SPARSEBUNDLEFS_USE_OPENSSL
#define SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
#define COMPARE_OPENSSL_AND_EMBEDDED_CRYPTO

#ifdef SPARSEBUNDLEFS_USE_OPENSSL
#  include <arpa/inet.h>
#  include "openssl/sha.h"
#  include "openssl/aes.h"
#  include "openssl/hmac.h"
#  include "openssl/evp.h"
#endif

#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
#  include "crypto/hmac-sha1/hmac/hmac.h"
#  include "crypto/PBKDF2_HMAC_SHA1.h"
#  include "crypto/Des.h"
#  include "crypto/TripleDes.h"
#  include "crypto/aes-rijndael/rijndael-aes.h"
#endif

#define HMACSHA1_KEY_SIZE 20 // from v2 header

class EncryptReader : public Reader
{
public:
	EncryptReader(std::shared_ptr<Reader> reader, std::string* passwordPtr);
	~EncryptReader();
	
	virtual int32_t read(void* buf, int32_t count, uint64_t offset);
	virtual uint64_t length();

private:
	void compute_iv(uint32_t chunk_no, uint8_t *iv);
	void decrypt_chunk(void *crypted_buffer, void* outputBuffer, uint32_t chunk_no);
	bool SetupEncryptionV2(std::shared_ptr<Reader> tokenReader, std::string* paswwordPtr);

	std::shared_ptr<Reader> m_reader;

	uint64_t m_crypt_offset;
	uint64_t m_crypt_size;
	int32_t m_crypt_blocksize;
//	uint64_t m_decrypt_size;

    uint8_t m_hmacsha1_key[HMACSHA1_KEY_SIZE];
	#ifdef SPARSEBUNDLEFS_USE_OPENSSL
		AES_KEY m_aes_decrypt_key;
	#endif
	#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
		aes_decrypt_ctx m_rijndael_ctx;
	#endif
    uint8_t m_aes_key[32]; // up to aes 256 bits
//    uint8_t m_aes_key_size;

};
