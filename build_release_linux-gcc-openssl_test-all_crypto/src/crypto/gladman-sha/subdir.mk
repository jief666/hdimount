################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/crypto/gladman-sha/hmac.c \
../src/crypto/gladman-sha/sha1.c \
../src/crypto/gladman-sha/sha2.c 

OBJS += \
./src/crypto/gladman-sha/hmac.o \
./src/crypto/gladman-sha/sha1.o \
./src/crypto/gladman-sha/sha2.o 

C_DEPS += \
./src/crypto/gladman-sha/hmac.d \
./src/crypto/gladman-sha/sha1.d \
./src/crypto/gladman-sha/sha2.d 


# Each subdirectory must supply rules for building sources it contributes
src/crypto/gladman-sha/%.o: ../src/crypto/gladman-sha/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	linux-gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


