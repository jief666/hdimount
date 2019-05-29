################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src_tests_encryption/crypto_openssl/test_AES_CBC_openssl.cpp \
../src_tests_encryption/crypto_openssl/test_AES_XTS_openssl.cpp \
../src_tests_encryption/crypto_openssl/test_DES_CBC_openssl.cpp \
../src_tests_encryption/crypto_openssl/test_HMAC_SHA1_openssl.cpp \
../src_tests_encryption/crypto_openssl/test_HMAC_SHA256_openssl.cpp \
../src_tests_encryption/crypto_openssl/test_PBKDF2_HMAC_SHA1_openssl.cpp \
../src_tests_encryption/crypto_openssl/test_PBKDF2_HMAC_SHA256_openssl.cpp \
../src_tests_encryption/crypto_openssl/test_aes_unwrap_key_openssl.cpp 

OBJS += \
./src_tests_encryption/crypto_openssl/test_AES_CBC_openssl.o \
./src_tests_encryption/crypto_openssl/test_AES_XTS_openssl.o \
./src_tests_encryption/crypto_openssl/test_DES_CBC_openssl.o \
./src_tests_encryption/crypto_openssl/test_HMAC_SHA1_openssl.o \
./src_tests_encryption/crypto_openssl/test_HMAC_SHA256_openssl.o \
./src_tests_encryption/crypto_openssl/test_PBKDF2_HMAC_SHA1_openssl.o \
./src_tests_encryption/crypto_openssl/test_PBKDF2_HMAC_SHA256_openssl.o \
./src_tests_encryption/crypto_openssl/test_aes_unwrap_key_openssl.o 

CPP_DEPS += \
./src_tests_encryption/crypto_openssl/test_AES_CBC_openssl.d \
./src_tests_encryption/crypto_openssl/test_AES_XTS_openssl.d \
./src_tests_encryption/crypto_openssl/test_DES_CBC_openssl.d \
./src_tests_encryption/crypto_openssl/test_HMAC_SHA1_openssl.d \
./src_tests_encryption/crypto_openssl/test_HMAC_SHA256_openssl.d \
./src_tests_encryption/crypto_openssl/test_PBKDF2_HMAC_SHA1_openssl.d \
./src_tests_encryption/crypto_openssl/test_PBKDF2_HMAC_SHA256_openssl.d \
./src_tests_encryption/crypto_openssl/test_aes_unwrap_key_openssl.d 


# Each subdirectory must supply rules for building sources it contributes
src_tests_encryption/crypto_openssl/%.o: ../src_tests_encryption/crypto_openssl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


