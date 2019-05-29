################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src_tests_encryption/test_crypto.cpp 

OBJS += \
./src_tests_encryption/test_crypto.o 

CPP_DEPS += \
./src_tests_encryption/test_crypto.d 


# Each subdirectory must supply rules for building sources it contributes
src_tests_encryption/%.o: ../src_tests_encryption/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


