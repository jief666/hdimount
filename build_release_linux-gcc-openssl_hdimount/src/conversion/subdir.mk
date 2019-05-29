################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/conversion/fast_unicode_compare_apple.cpp \
../src/conversion/utf16Tables.cpp \
../src/conversion/utf816Conversion.cpp 

OBJS += \
./src/conversion/fast_unicode_compare_apple.o \
./src/conversion/utf16Tables.o \
./src/conversion/utf816Conversion.o 

CPP_DEPS += \
./src/conversion/fast_unicode_compare_apple.d \
./src/conversion/utf16Tables.d \
./src/conversion/utf816Conversion.d 


# Each subdirectory must supply rules for building sources it contributes
src/conversion/%.o: ../src/conversion/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


