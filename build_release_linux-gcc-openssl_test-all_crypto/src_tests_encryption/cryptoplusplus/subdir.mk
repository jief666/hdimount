################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src_tests_encryption/cryptoplusplus/test_AES_CBC_cplusplus.cpp \
../src_tests_encryption/cryptoplusplus/test_AES_XTS_cplusplus.cpp \
../src_tests_encryption/cryptoplusplus/test_DES_CBC_cplusplus.cpp \
../src_tests_encryption/cryptoplusplus/test_HMAC_SHA1_cplusplus.cpp \
../src_tests_encryption/cryptoplusplus/test_HMAC_SHA256_cplusplus.cpp \
../src_tests_encryption/cryptoplusplus/test_PBKDF2_HMAC_SHA1_cplusplus.cpp \
../src_tests_encryption/cryptoplusplus/test_PBKDF2_HMAC_SHA256_cplusplus.cpp \
../src_tests_encryption/cryptoplusplus/test_aes_unwrap_key_cplusplus.cpp 

OBJS += \
./src_tests_encryption/cryptoplusplus/test_AES_CBC_cplusplus.o \
./src_tests_encryption/cryptoplusplus/test_AES_XTS_cplusplus.o \
./src_tests_encryption/cryptoplusplus/test_DES_CBC_cplusplus.o \
./src_tests_encryption/cryptoplusplus/test_HMAC_SHA1_cplusplus.o \
./src_tests_encryption/cryptoplusplus/test_HMAC_SHA256_cplusplus.o \
./src_tests_encryption/cryptoplusplus/test_PBKDF2_HMAC_SHA1_cplusplus.o \
./src_tests_encryption/cryptoplusplus/test_PBKDF2_HMAC_SHA256_cplusplus.o \
./src_tests_encryption/cryptoplusplus/test_aes_unwrap_key_cplusplus.o 

CPP_DEPS += \
./src_tests_encryption/cryptoplusplus/test_AES_CBC_cplusplus.d \
./src_tests_encryption/cryptoplusplus/test_AES_XTS_cplusplus.d \
./src_tests_encryption/cryptoplusplus/test_DES_CBC_cplusplus.d \
./src_tests_encryption/cryptoplusplus/test_HMAC_SHA1_cplusplus.d \
./src_tests_encryption/cryptoplusplus/test_HMAC_SHA256_cplusplus.d \
./src_tests_encryption/cryptoplusplus/test_PBKDF2_HMAC_SHA1_cplusplus.d \
./src_tests_encryption/cryptoplusplus/test_PBKDF2_HMAC_SHA256_cplusplus.d \
./src_tests_encryption/cryptoplusplus/test_aes_unwrap_key_cplusplus.d 


# Each subdirectory must supply rules for building sources it contributes
src_tests_encryption/cryptoplusplus/%.o: ../src_tests_encryption/cryptoplusplus/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


