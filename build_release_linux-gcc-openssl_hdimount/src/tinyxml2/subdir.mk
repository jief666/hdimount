################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tinyxml2/tinyxml2.cpp 

OBJS += \
./src/tinyxml2/tinyxml2.o 

CPP_DEPS += \
./src/tinyxml2/tinyxml2.d 


# Each subdirectory must supply rules for building sources it contributes
src/tinyxml2/%.o: ../src/tinyxml2/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


