################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/crypto/gladman-aes-modes/xts.c 

OBJS += \
./src/crypto/gladman-aes-modes/xts.o 

C_DEPS += \
./src/crypto/gladman-aes-modes/xts.d 


# Each subdirectory must supply rules for building sources it contributes
src/crypto/gladman-aes-modes/%.o: ../src/crypto/gladman-aes-modes/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	linux-gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


