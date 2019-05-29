################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src_tests_diskimages/compare_di_with_constants/DiskImageComparisonConstants.cpp \
../src_tests_diskimages/compare_di_with_constants/compare_di_with_constants.cpp 

OBJS += \
./src_tests_diskimages/compare_di_with_constants/DiskImageComparisonConstants.o \
./src_tests_diskimages/compare_di_with_constants/compare_di_with_constants.o 

CPP_DEPS += \
./src_tests_diskimages/compare_di_with_constants/DiskImageComparisonConstants.d \
./src_tests_diskimages/compare_di_with_constants/compare_di_with_constants.d 


# Each subdirectory must supply rules for building sources it contributes
src_tests_diskimages/compare_di_with_constants/%.o: ../src_tests_diskimages/compare_di_with_constants/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


