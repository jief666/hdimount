
extern int test_aes_unwrap_key();
extern int test_PBKDF2_HMAC_SHA1();
extern int test_PBKDF2_HMAC_SHA256();
extern int test_DES_CBC();
extern int test_HMAC_SHA1();
extern int test_HMAC_SHA256();
extern int test_HMAC_SHA1_plusplus();
extern int test_HMAC_SHA256_plusplus();

extern int test_AES_CBC_openssl();
extern int test_AES_CBC_crypto();
extern int test_AES_CBC_plusplus();

extern int test_AES_XTS_openssl();
extern int test_AES_XTS_crypto();
extern int test_AES_XTS_plusplus();

int main(int argc, char **argv)
{
	test_aes_unwrap_key();
	test_PBKDF2_HMAC_SHA1();
	test_PBKDF2_HMAC_SHA256();
	test_DES_CBC();

	test_HMAC_SHA1();
	test_HMAC_SHA1_plusplus();
	test_HMAC_SHA256();
	test_HMAC_SHA256_plusplus();

	
	test_AES_CBC_openssl();
	test_AES_CBC_crypto();
	test_AES_CBC_plusplus();
	
	test_AES_XTS_openssl();
	test_AES_XTS_crypto();
	test_AES_XTS_plusplus();
}

