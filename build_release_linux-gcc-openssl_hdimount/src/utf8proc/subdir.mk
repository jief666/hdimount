################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/utf8proc/utf8proc.c 

OBJS += \
./src/utf8proc/utf8proc.o 

C_DEPS += \
./src/utf8proc/utf8proc.d 


# Each subdirectory must supply rules for building sources it contributes
src/utf8proc/%.o: ../src/utf8proc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	linux-gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


