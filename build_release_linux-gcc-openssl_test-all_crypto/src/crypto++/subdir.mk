################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/crypto++/Aes++.cpp \
../src/crypto++/AesXts.cpp \
../src/crypto++/Crc32++.cpp \
../src/crypto++/Crypto.cpp \
../src/crypto++/Des++.cpp \
../src/crypto++/Sha1++.cpp \
../src/crypto++/Sha256.cpp \
../src/crypto++/TripleDes++.cpp 

OBJS += \
./src/crypto++/Aes++.o \
./src/crypto++/AesXts.o \
./src/crypto++/Crc32++.o \
./src/crypto++/Crypto.o \
./src/crypto++/Des++.o \
./src/crypto++/Sha1++.o \
./src/crypto++/Sha256.o \
./src/crypto++/TripleDes++.o 

CPP_DEPS += \
./src/crypto++/Aes++.d \
./src/crypto++/AesXts.d \
./src/crypto++/Crc32++.d \
./src/crypto++/Crypto.d \
./src/crypto++/Des++.d \
./src/crypto++/Sha1++.d \
./src/crypto++/Sha256.d \
./src/crypto++/TripleDes++.d 


# Each subdirectory must supply rules for building sources it contributes
src/crypto++/%.o: ../src/crypto++/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


