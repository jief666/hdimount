/*
This file is based on the work of Simon Gander (https://github.com/sgan81/apfs-fuse)
Copyright (C) 2018 Jief Luce
Copyright (C) 2017 Simon Gander

You should have received a copy of the GNU General Public License
along with hdimount.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "EncryptReader.h"

#include <cstring>
#include <cassert>
#include <vector>
#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h> // for strtoumax
#include <fcntl.h> // open, read, close...

#include "Utils.hpp"
#include "../darling-dmg/src/exceptions.h"
#include "../darling-dmg/src/be.h"

//------------------------------------------- Crypto choice
// I tried tiny-AES-C from https://github.com/kokke/tiny-AES-c.git but it was very slow.

#if !defined(SPARSEBUNDLEFS_USE_OPENSSL) && !defined(SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO)
#  define SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO // default : use embedded crypto
#endif

#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
#  ifdef SPARSEBUNDLEFS_USE_OPENSSL
#    define COMPARE_OPENSSL_AND_EMBEDDED_CRYPTO
#  endif
#endif

//------------------------------------------- Crypto include + defines
#ifdef SPARSEBUNDLEFS_USE_OPENSSL
#  include <arpa/inet.h>
#  include "openssl/sha.h"
#  include "openssl/aes.h"
#  include "openssl/hmac.h"
#  include "openssl/evp.h"
#endif

//#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
//#  include "../crypto/hmac-sha1/hmac/hmac.h"
//#  include "../crypto/PBKDF2_HMAC_SHA1.h"
//#  include "../crypto/Des.h"
//#  include "../crypto/TripleDes.h"
//#  include "../crypto/aes-rijndael/rijndael-aes.h"
//#endif

/* length of message digest output in bytes (160 bits) */
#define MD_LENGTH		20
/* block size of cipher in bytes (128 bits) */
#define CIPHER_BLOCKSIZE	16


#pragma pack(push, 1)

struct DmgCryptHeaderV2
{
	char signature[8];
	be_jief<uint32_t> maybe_version;
	be_jief<uint32_t> enc_iv_size;
	be_jief<uint32_t> encMode;
	be_jief<uint32_t> encAlg;
	be_jief<uint32_t> key_bits;
	be_jief<uint32_t> prngalg;
	be_jief<uint32_t> prngkeysize;
	uint8_t uuid[0x10];
	be_jief<uint32_t> block_size;
	be_jief<uint64_t> encrypted_data_length;
	be_jief<uint64_t> encrypted_data_offset;
	be_jief<uint32_t> no_of_keys;
};

struct DmgKeyPointer
{
	be_jief<uint32_t> key_type;
	be_jief<uint64_t> key_offset;
	be_jief<uint64_t> key_length;
};

struct DmgKeyData
{
	be_jief<uint32_t> kdf_algorithm;
	be_jief<uint32_t> prng_algorithm;
	be_jief<uint32_t> iteration_count;
	be_jief<uint32_t> salt_len;
	uint8_t salt[0x20];
	be_jief<uint32_t> blob_enc_iv_size;
	uint8_t blob_enc_iv[0x20];
	be_jief<uint32_t> blob_enc_key_bits;
	be_jief<uint32_t> blob_enc_algorithm;
	be_jief<uint32_t> blob_enc_padding;
	be_jief<uint32_t> blob_enc_mode;
	be_jief<uint32_t> encr_key_blob_size;
	uint8_t encr_key_blob[0x200];
};

#pragma pack(pop)

bool EncryptReader::SetupEncryptionV2(std::shared_ptr<Reader> tokenReader, std::string* passwordPtr)
{
	char data[0x1000];
	std::vector<uint8_t> kdata;

	const DmgCryptHeaderV2 *hdr;
	const DmgKeyPointer *keyptr;
	const DmgKeyData *keydata;
	uint32_t no_of_keys;
	uint32_t key_id;

	tokenReader->read(data, sizeof(data), 0);

	assert(sizeof(data) > sizeof(DmgCryptHeaderV2));
	hdr = reinterpret_cast<const DmgCryptHeaderV2 *>(data);

	if (memcmp(hdr->signature, "encrcdsa", 8))
		return false;

	m_crypt_offset = hdr->encrypted_data_offset;
	m_crypt_size = hdr->encrypted_data_length;
	if (hdr->block_size > INT_MAX)
		throw io_error(stprintf("Encrypted block sizeis too big : %d. Usually 512 or 4096", m_crypt_blocksize));
	m_crypt_blocksize = (int32_t)hdr->block_size;

	std::string password;
	if (!passwordPtr) {
		if (!GetPassword(&password))
			throw io_error("Failed to get password");
		passwordPtr = &password;
	}
	
	no_of_keys = hdr->no_of_keys;

	for (key_id = 0; key_id < no_of_keys; key_id++)
	{
		assert(sizeof(data) > sizeof(DmgCryptHeaderV2) + key_id * sizeof(DmgKeyPointer) + sizeof(DmgKeyPointer));
		keyptr = reinterpret_cast<const DmgKeyPointer *>(data + sizeof(DmgCryptHeaderV2) + key_id * sizeof(DmgKeyPointer));

		if (keyptr->key_length > INT32_MAX)
			throw io_error(stprintf("Key too big (%d bytes)"));

		kdata.resize(keyptr->key_length);
//		m_dmg.seekg(keyptr->key_offset.get());
//		m_dmg.read(reinterpret_cast<char *>(kdata.data()), kdata.size());
		int32_t bytes_read = tokenReader->read(reinterpret_cast<char *>(kdata.data()), kdata.size(), keyptr->key_offset.get());
		if ( bytes_read != (int32_t)kdata.size())
			throw io_error(stprintf("Cannot read %d bytes at offset %d. Returns %d", kdata.size(), keyptr->key_offset.get(), bytes_read));

		keydata = reinterpret_cast<const DmgKeyData *>(kdata.data());

		#ifdef SPARSEBUNDLEFS_USE_OPENSSL
			uint8_t derived_key_openssl[192/8];
			EVP_CIPHER_CTX ctx;
			int outlen, tmplen;
		#endif
		#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
			uint8_t derived_key[192/8];
		#endif
		
		#ifdef SPARSEBUNDLEFS_USE_OPENSSL
			PKCS5_PBKDF2_HMAC_SHA1(passwordPtr->c_str(), passwordPtr->length(), (unsigned char*)keydata->salt, keydata->salt_len, keydata->iteration_count, sizeof(derived_key_openssl), derived_key_openssl);
		#endif
		#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
			PBKDF2_HMAC_SHA1((const uint8_t *)(passwordPtr->c_str()), passwordPtr->length(), (unsigned char*)keydata->salt, keydata->salt_len, keydata->iteration_count, derived_key, sizeof(derived_key));
		#endif
//		PBKDF2_HMAC_SHA1(reinterpret_cast<const uint8_t *>(password.c_str()), password.size(), keydata->salt, keydata->salt_len, keydata->iteration_count, derived_key, sizeof(derived_key));
		#ifdef COMPARE_OPENSSL_AND_EMBEDDED_CRYPTO
			if ( memcmp(derived_key_openssl, derived_key, sizeof(derived_key_openssl)) != 0 ) {
				printf("PKCS5_PBKDF2_HMAC_SHA1 doesn't give the same result with embedded crypto\n");
			}
		#endif

		uint32_t blob_len = keydata->encr_key_blob_size;
		uint8_t blob[blob_len]; // /* result of the decryption operation shouldn't be bigger than ciphertext */

		#ifdef SPARSEBUNDLEFS_USE_OPENSSL
	
			EVP_CIPHER_CTX_init(&ctx);
			EVP_DecryptInit_ex(&ctx, EVP_des_ede3_cbc(), NULL, derived_key_openssl, keydata->blob_enc_iv);

			if(!EVP_DecryptUpdate(&ctx, blob, &outlen, keydata->encr_key_blob, blob_len)) {
				throw io_error("internal error (1) during key unwrap operation!");
			}
			if(!EVP_DecryptFinal_ex(&ctx, blob + outlen, &tmplen)) {
				throw io_error("internal error (2) during key unwrap operation!");
			}
			outlen += tmplen;
			EVP_CIPHER_CTX_cleanup(&ctx);

			memcpy((void*)&m_aes_key, blob, hdr->key_bits/8);
			memcpy(m_hmacsha1_key, blob+hdr->key_bits/8, HMACSHA1_KEY_SIZE);
    		AES_set_decrypt_key(m_aes_key, hdr->key_bits, &m_aes_decrypt_key);

		#endif
		#ifdef COMPARE_OPENSSL_AND_EMBEDDED_CRYPTO
			uint8_t blobOpenssl[blob_len]; // /* result of the decryption operation shouldn't be bigger than ciphertext */
			memcpy(blobOpenssl, blob, blob_len);
		#endif

		#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO

			TripleDesInit();
			TripleDesSetKey(derived_key);
			TripleDesSetIV(keydata->blob_enc_iv);

		//	uint32_t blob_len = pwhdr->encrypted_keyblob_size;

			TripleDesDecryptCBC(blob, keydata->encr_key_blob, blob_len);

			memcpy(m_aes_key, blob, hdr->key_bits/8);
			memcpy(m_hmacsha1_key, blob+hdr->key_bits/8, HMACSHA1_KEY_SIZE);

    		if ( hdr->key_bits/8 == 16 ) aes_decrypt_key128(m_aes_key, &m_rijndael_ctx);
    		if ( hdr->key_bits/8 == 32 ) aes_decrypt_key256(m_aes_key, &m_rijndael_ctx);

		#endif
		#ifdef COMPARE_OPENSSL_AND_EMBEDDED_CRYPTO
			if ( memcmp(blobOpenssl, blob, blob_len) != 0 ) {
				printf("Triple doesn't give the same result with embedded crypto\n");
			}
		#endif
//
//		des.SetKey(derived_key);
//		des.SetIV(keydata->blob_enc_iv);

//		des.DecryptCBC(blob, keydata->encr_key_blob, blob_len);

		if (blob[blob_len - 1] < 1 || blob[blob_len - 1] > 8)
			continue;
		blob_len -= blob[blob_len - 1];

		if (memcmp(blob + blob_len - 5, "CKIE", 4))
			continue;

		return true;
//		if (hdr->key_bits == 128)
//		{
//			m_aes.SetKey(blob, AES::AES_128);
//			memcpy(m_hmac_key, blob + 0x10, 0x14);
//			key_ok = true;
//			break;
//		}
//		else if (hdr->key_bits == 256)
//		{
//			m_aes.SetKey(blob, AES::AES_256);
//			memcpy(m_hmac_key, blob + 0x20, 0x14);
//			key_ok = true;
//			break;
//		}
	}

	return false;
}

EncryptReader::EncryptReader(std::shared_ptr<Reader> reader, std::string* passwordPtr)
: m_reader(reader)
{
	
	if (!SetupEncryptionV2(reader, passwordPtr))
	{
		throw io_error("Not encrypted");
	}
}

EncryptReader::~EncryptReader()
{
}

void EncryptReader::compute_iv(uint32_t blkid, uint8_t *iv)
{
		blkid = bswap_h_to_be(blkid);

//			HMAC_SHA1(m_hmac_key, 0x14, reinterpret_cast<const uint8_t *>(&blkid), sizeof(uint32_t), iv);
	#ifdef SPARSEBUNDLEFS_USE_OPENSSL
		unsigned char mdResultOpenSsl[MD_LENGTH];
		unsigned int mdLenOpenSsl;

		HMAC_CTX hmacsha1_ctx;
		HMAC_CTX_init(&hmacsha1_ctx);
		HMAC_Init_ex(&hmacsha1_ctx, m_hmacsha1_key, sizeof(m_hmacsha1_key), EVP_sha1(), NULL);
		HMAC_Update(&hmacsha1_ctx, (const unsigned char *) &blkid, sizeof(blkid));
		HMAC_Final(&hmacsha1_ctx, mdResultOpenSsl, &mdLenOpenSsl);
		HMAC_CTX_cleanup(&hmacsha1_ctx);
		memcpy(iv, mdResultOpenSsl, CIPHER_BLOCKSIZE); // TODO avoid memory copy
	#endif
	#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
		unsigned char mdResult2[MD_LENGTH];
		size_t mdLen2;
		mdLen2 = sizeof(mdResult2);
		hmac_sha1(m_hmacsha1_key, sizeof(m_hmacsha1_key), (const unsigned char *) &blkid, sizeof(blkid), mdResult2, &mdLen2);
		memcpy(iv, mdResult2, CIPHER_BLOCKSIZE); // TODO avoid memory copy
	#endif
}

void EncryptReader::decrypt_chunk(void *crypted_buffer, void* outputBuffer, uint32_t blkid)
{
	#if defined(SPARSEBUNDLEFS_USE_OPENSSL) && defined(SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO)
		uint8_t crypted_buffer_sav[m_crypt_blocksize];
		uint8_t decrypted_buffer_openssl[m_crypt_blocksize];
		memcpy(crypted_buffer_sav, crypted_buffer, m_crypt_blocksize);
	#endif

	uint8_t iv[CIPHER_BLOCKSIZE];

	#ifdef SPARSEBUNDLEFS_USE_OPENSSL
//		uint8_t decrypted_buffer_openssl[m_crypt_blocksize];
//print_hex(iv, CIPHER_BLOCKSIZE, "decrypt_chunk  chunk_no=%d, iv=", chunk_no);
//print_hex(sparsebundle_data->aes_key, sparsebundle_data->aes_key_size, "aes key=");
		compute_iv(blkid, iv);
		AES_cbc_encrypt((uint8_t*)crypted_buffer, (uint8_t*)outputBuffer, m_crypt_blocksize, &m_aes_decrypt_key, iv, AES_DECRYPT);
//print_hex(crypted_buffer, sparsebundle_data->blocksize, "crypted_buffer=");
//print_hex(decrypted_buffer_openssl, sparsebundle_data->blocksize, "decrypted_buffer_openssl=");
	#endif

	#if defined(COMPARE_OPENSSL_AND_EMBEDDED_CRYPTO)
		memcpy(decrypted_buffer_openssl, outputBuffer, m_crypt_blocksize);
	#endif

	#ifdef SPARSEBUNDLEFS_USE_EMBEDDED_CRYPTO
		uint8_t rijndael_decrypted_buffer[m_crypt_blocksize];
		compute_iv(blkid, iv);
		aes_cbc_decrypt((uint8_t*)crypted_buffer, (uint8_t*)outputBuffer, m_crypt_blocksize, iv, &m_rijndael_ctx);
		memcpy(rijndael_decrypted_buffer, outputBuffer, m_crypt_blocksize);
	#endif

	#if defined(COMPARE_OPENSSL_AND_EMBEDDED_CRYPTO)
		if ( memcmp(decrypted_buffer_openssl, rijndael_decrypted_buffer, m_crypt_blocksize) != 0 ) {
			printf("decrypt_chunk OpenSsl != rijndael\n");
		}
	#endif
}
int32_t EncryptReader::read(void* outputBuffer, int32_t size2, uint64_t off)
{
	uint8_t buffer[m_crypt_blocksize];
	uint8_t outputBufferTmp[m_crypt_blocksize];
	uint64_t mask = m_crypt_blocksize - 1;
	uint32_t blkid;
	int32_t rd_len;
	uint8_t *bdata = reinterpret_cast<uint8_t *>(outputBuffer);
	int32_t bytesLeft = size2;

	if (off & mask)
	{
		blkid = static_cast<uint32_t>(off / m_crypt_blocksize);

		if ( m_reader->read(buffer, m_crypt_blocksize, m_crypt_offset + (off & ~mask)) != m_crypt_blocksize )
			return int32_t(bdata - (uint8_t*)outputBuffer); // cast ok because it's not > size
//			m_dmg.seekg(m_crypt_offset + (off & ~mask));
//			m_dmg.read(reinterpret_cast<char *>(buffer), m_crypt_blocksize);


//			m_aes.SetIV(iv);
//			m_aes.DecryptCBC(buffer, buffer, m_crypt_blocksize);

		decrypt_chunk(buffer, outputBufferTmp, blkid);

		rd_len = m_crypt_blocksize - (off & mask);
		if (rd_len > bytesLeft)
			rd_len = bytesLeft;

		memcpy(bdata, outputBufferTmp + (off & mask), rd_len);

		bdata += rd_len;
		off += rd_len;
		bytesLeft -= rd_len;
	}

	while (bytesLeft > m_crypt_blocksize)
	{
		blkid = static_cast<uint32_t>(off / m_crypt_blocksize);
//			blkid = bswap_be(blkid);


		if ( m_reader->read(buffer, m_crypt_blocksize, m_crypt_offset + (off & ~mask)) != m_crypt_blocksize ) // TODO can we remove & ~mask ?
			return int32_t(bdata - (uint8_t*)outputBuffer); // cast ok because it's not > size

		decrypt_chunk(buffer, bdata, blkid);

		rd_len = m_crypt_blocksize;

		bdata += rd_len;
		off += rd_len;
		bytesLeft -= rd_len;
	}

	blkid = static_cast<uint32_t>(off / m_crypt_blocksize);
//			blkid = bswap_be(blkid);


	if ( m_reader->read(buffer, m_crypt_blocksize, m_crypt_offset + (off & ~mask)) != m_crypt_blocksize ) // TODO can we remove & ~mask ?
		return int32_t(bdata - (uint8_t*)outputBuffer); // cast ok because it's not > size
//			ReadRaw(buffer, m_crypt_blocksize, m_crypt_offset + (off & ~mask));
//			m_dmg.seekg(m_crypt_offset + (off & ~mask));
//			m_dmg.read(reinterpret_cast<char *>(buffer), m_crypt_blocksize);

//			compute_iv(blkid, iv);
//			HMAC_SHA1(m_hmac_key, 0x14, reinterpret_cast<const uint8_t *>(&blkid), sizeof(uint32_t), iv);

//			m_aes.SetIV(iv);
//			m_aes.DecryptCBC(buffer, buffer, m_crypt_blocksize);

	decrypt_chunk(buffer, outputBufferTmp, blkid);

	memcpy(bdata, outputBufferTmp, bytesLeft);

	return size2;
}

uint64_t EncryptReader::length()
{
	return m_reader->length() - m_crypt_offset;
}
