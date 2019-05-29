################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/crypto_openssl/crc32c.c 

CPP_SRCS += \
../src/crypto_openssl/APFSLibCryptoOpenssl.cpp \
../src/crypto_openssl/DarlingDMGCryptoOpenssl.cpp 

OBJS += \
./src/crypto_openssl/APFSLibCryptoOpenssl.o \
./src/crypto_openssl/DarlingDMGCryptoOpenssl.o \
./src/crypto_openssl/crc32c.o 

C_DEPS += \
./src/crypto_openssl/crc32c.d 

CPP_DEPS += \
./src/crypto_openssl/APFSLibCryptoOpenssl.d \
./src/crypto_openssl/DarlingDMGCryptoOpenssl.d 


# Each subdirectory must supply rules for building sources it contributes
src/crypto_openssl/%.o: ../src/crypto_openssl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/crypto_openssl/%.o: ../src/crypto_openssl/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	linux-gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


