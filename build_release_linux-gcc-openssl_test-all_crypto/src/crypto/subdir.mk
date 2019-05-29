################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/crypto/Des.c \
../src/crypto/PBKDF2_HMAC_SHA1.c \
../src/crypto/PBKDF2_HMAC_SHA256.c \
../src/crypto/TripleDes.c \
../src/crypto/crc32c.c 

CPP_SRCS += \
../src/crypto/Rfc3394.cpp 

OBJS += \
./src/crypto/Des.o \
./src/crypto/PBKDF2_HMAC_SHA1.o \
./src/crypto/PBKDF2_HMAC_SHA256.o \
./src/crypto/Rfc3394.o \
./src/crypto/TripleDes.o \
./src/crypto/crc32c.o 

C_DEPS += \
./src/crypto/Des.d \
./src/crypto/PBKDF2_HMAC_SHA1.d \
./src/crypto/PBKDF2_HMAC_SHA256.d \
./src/crypto/TripleDes.d \
./src/crypto/crc32c.d 

CPP_DEPS += \
./src/crypto/Rfc3394.d 


# Each subdirectory must supply rules for building sources it contributes
src/crypto/%.o: ../src/crypto/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	linux-gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/crypto/%.o: ../src/crypto/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


