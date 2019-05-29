################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ApfsFuseVolume.cpp \
../src/CrossPlatform.cpp \
../src/FuseVolume.cpp \
../src/HfsFuseVolume.cpp \
../src/MBRDisk.cpp \
../src/OpenDisk.cpp \
../src/RawDisk.cpp \
../src/SparsebundleReader.cpp \
../src/SparseimageDisk.cpp \
../src/Utils.cpp 

OBJS += \
./src/ApfsFuseVolume.o \
./src/CrossPlatform.o \
./src/FuseVolume.o \
./src/HfsFuseVolume.o \
./src/MBRDisk.o \
./src/OpenDisk.o \
./src/RawDisk.o \
./src/SparsebundleReader.o \
./src/SparseimageDisk.o \
./src/Utils.o 

CPP_DEPS += \
./src/ApfsFuseVolume.d \
./src/CrossPlatform.d \
./src/FuseVolume.d \
./src/HfsFuseVolume.d \
./src/MBRDisk.d \
./src/OpenDisk.d \
./src/RawDisk.d \
./src/SparsebundleReader.d \
./src/SparseimageDisk.d \
./src/Utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


