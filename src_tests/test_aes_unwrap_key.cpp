#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>

#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/modes.h>

#include "../src/crypto/Rfc3394.h"
#include "../src/crypto++/Crypto.h"

int AES_wrap_unwrap_test_openssl(const unsigned char *kek, int keybits,
			 const unsigned char *wrapped_key,
			 int wrapped_key_len,
			 const unsigned char *iv,
			 const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char ptmp[256];
	assert(sizeof(ptmp) > expected_result_len);

	int r;
	AES_KEY wctx;
		
	if (AES_set_decrypt_key(kek, keybits, &wctx))
		goto err;
	r = AES_unwrap_key(&wctx, iv, ptmp, wrapped_key, wrapped_key_len);

	if ( r != expected_result_len )
		goto err;
	if (memcmp(expected_result, ptmp, expected_result_len))
		goto err;

	ret = 1;

	err:

	return ret;

}

int AES_wrap_unwrap_test_crypto(const unsigned char *kek, int keybits,
			 const unsigned char *wrapped_key,
			 int wrapped_key_len,
			 const unsigned char *iv,
			 const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char ptmp[256];
	assert(sizeof(ptmp) > expected_result_len);

	int r = AES_unwrap_key_2(ptmp, wrapped_key, wrapped_key_len, kek, keybits, NULL);

	if ( r != expected_result_len )
		goto err;
	if (memcmp(expected_result, ptmp, expected_result_len))
		goto err;

	ret = 1;

	err:

	return ret;

}

int AES_wrap_unwrap_test_cryptoplusplus(const unsigned char *kek, int keybits,
			 const unsigned char *wrapped_key,
			 int wrapped_key_len,
			 const unsigned char *iv,
			 const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char ptmp[256];
	assert(sizeof(ptmp) > expected_result_len);

	auto aes_mode = AES::AES_128;
	if ( keybits == 256 ) aes_mode = AES::AES_256;
	int r = Rfc3394_KeyUnwrap(ptmp, wrapped_key, expected_result_len, kek, aes_mode, NULL);

	if ( r != 1 )
		goto err;
	if (memcmp(expected_result, ptmp, expected_result_len))
		goto err;

	ret = 1;

	err:

	return ret;

}

int test_aes_unwrap_key()
{

//static const unsigned char kek[] = {
//  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
//  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
//  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
//  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
//};
//
//static const unsigned char wrapped_key[] = {
//  0x28, 0xc9, 0xf4, 0x04, 0xc4, 0xb8, 0x10, 0xf4,
//  0xcb, 0xcc, 0xb3, 0x5c, 0xfb, 0x87, 0xf8, 0x26,
//  0x3f, 0x57, 0x86, 0xe2, 0xd8, 0x0e, 0xd3, 0x26,
//  0xcb, 0xc7, 0xf0, 0xe7, 0x1a, 0x99, 0xf4, 0x3b,
//  0xfb, 0x98, 0x8b, 0x9b, 0x7a, 0x02, 0xdd, 0x21
//};
//
//static const unsigned char unwrapped_key[] = { // this is the expected result
//  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
//  0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
//  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
//  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
//};
static const uint8_t kek[] = {
	0x52, 0x7b, 0x9a, 0xbd, 0x41, 0x0a, 0x69, 0x93,
	0x11, 0x0e, 0x6c, 0x5f, 0xdf, 0x6f, 0x25, 0xff,
	0xa5, 0xcb, 0xb1, 0xdb, 0x98, 0xbe, 0x18, 0xe3,
	0x3d, 0x45, 0xa5, 0xe3, 0x9e, 0xe7, 0x0d, 0x4c
};
static const uint8_t wrapped_key[] = {
	0xaa, 0xa8, 0x39, 0xcb, 0xe7, 0x17, 0x3b, 0x59,
	0x07, 0xe3, 0xc7, 0x96, 0xa1, 0x1b, 0xfa, 0x1a,
	0xd7, 0x51, 0xc1, 0x9e, 0x54, 0x30, 0xbd, 0x29,
	0xb1, 0x23, 0x9a, 0xf6, 0xa2, 0xff, 0xf3, 0x60,
	0x17, 0x46, 0xbe, 0x9f, 0x4d, 0x89, 0xbd, 0x9a
};
static const uint8_t unwrapped_key[] = {
	0xf0, 0x60, 0x5c, 0xc3, 0xf1, 0xed, 0x03, 0x05,
	0xb7, 0x03, 0x93, 0x75, 0x2e, 0x90, 0xd6, 0x92,
	0xcb, 0x4e, 0x04, 0xec, 0x96, 0x3c, 0x4b, 0x75,
	0xed, 0xef, 0xc2, 0xbd, 0x62, 0x0c, 0x6b, 0xf8
};

	bool all_tests_ok = true;
	int ret;
	ret = AES_wrap_unwrap_test_openssl(kek, 256, wrapped_key, sizeof(wrapped_key), NULL, unwrapped_key, 32);
	fprintf(stderr, "AES_wrap_unwrap_test_openssl %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	ret = AES_wrap_unwrap_test_crypto(kek, 256, wrapped_key, sizeof(wrapped_key), NULL, unwrapped_key, 32);
	fprintf(stderr, "AES_wrap_unwrap_test_crypto %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	ret = AES_wrap_unwrap_test_cryptoplusplus(kek, 256, wrapped_key, sizeof(wrapped_key), NULL, unwrapped_key, 32);
	fprintf(stderr, "AES_wrap_unwrap_test_cryptoplusplus %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;


	return 1;
}

