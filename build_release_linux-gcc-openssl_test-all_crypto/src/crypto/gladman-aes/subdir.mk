################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/crypto/gladman-aes/aes_modes.c \
../src/crypto/gladman-aes/aes_ni.c \
../src/crypto/gladman-aes/aescrypt.c \
../src/crypto/gladman-aes/aeskey.c \
../src/crypto/gladman-aes/aestab.c 

OBJS += \
./src/crypto/gladman-aes/aes_modes.o \
./src/crypto/gladman-aes/aes_ni.o \
./src/crypto/gladman-aes/aescrypt.o \
./src/crypto/gladman-aes/aeskey.o \
./src/crypto/gladman-aes/aestab.o 

C_DEPS += \
./src/crypto/gladman-aes/aes_modes.d \
./src/crypto/gladman-aes/aes_ni.d \
./src/crypto/gladman-aes/aescrypt.d \
./src/crypto/gladman-aes/aeskey.d \
./src/crypto/gladman-aes/aestab.d 


# Each subdirectory must supply rules for building sources it contributes
src/crypto/gladman-aes/%.o: ../src/crypto/gladman-aes/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	linux-gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


