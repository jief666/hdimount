################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src_tests_encryption/crypto/test_AES_CBC_crypto_c.cpp \
../src_tests_encryption/crypto/test_AES_XTS_crypto_c.cpp \
../src_tests_encryption/crypto/test_DES_CBC_crypto_c.cpp \
../src_tests_encryption/crypto/test_HMAC_SHA1_crypto_c.cpp \
../src_tests_encryption/crypto/test_HMAC_SHA256_crypto_c.cpp \
../src_tests_encryption/crypto/test_PBKDF2_HMAC_SHA1_crypto_c.cpp \
../src_tests_encryption/crypto/test_PBKDF2_HMAC_SHA256_crypto_c.cpp \
../src_tests_encryption/crypto/test_aes_unwrap_key_crypto_c.cpp 

OBJS += \
./src_tests_encryption/crypto/test_AES_CBC_crypto_c.o \
./src_tests_encryption/crypto/test_AES_XTS_crypto_c.o \
./src_tests_encryption/crypto/test_DES_CBC_crypto_c.o \
./src_tests_encryption/crypto/test_HMAC_SHA1_crypto_c.o \
./src_tests_encryption/crypto/test_HMAC_SHA256_crypto_c.o \
./src_tests_encryption/crypto/test_PBKDF2_HMAC_SHA1_crypto_c.o \
./src_tests_encryption/crypto/test_PBKDF2_HMAC_SHA256_crypto_c.o \
./src_tests_encryption/crypto/test_aes_unwrap_key_crypto_c.o 

CPP_DEPS += \
./src_tests_encryption/crypto/test_AES_CBC_crypto_c.d \
./src_tests_encryption/crypto/test_AES_XTS_crypto_c.d \
./src_tests_encryption/crypto/test_DES_CBC_crypto_c.d \
./src_tests_encryption/crypto/test_HMAC_SHA1_crypto_c.d \
./src_tests_encryption/crypto/test_HMAC_SHA256_crypto_c.d \
./src_tests_encryption/crypto/test_PBKDF2_HMAC_SHA1_crypto_c.d \
./src_tests_encryption/crypto/test_PBKDF2_HMAC_SHA256_crypto_c.d \
./src_tests_encryption/crypto/test_aes_unwrap_key_crypto_c.d 


# Each subdirectory must supply rules for building sources it contributes
src_tests_encryption/crypto/%.o: ../src_tests_encryption/crypto/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


