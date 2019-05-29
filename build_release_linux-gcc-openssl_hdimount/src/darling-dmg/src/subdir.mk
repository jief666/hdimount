################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/darling-dmg/src/AppleDisk.cpp \
../src/darling-dmg/src/CacheZone.cpp \
../src/darling-dmg/src/CachedReader.cpp \
../src/darling-dmg/src/DMGDecompressor.cpp \
../src/darling-dmg/src/DMGDisk.cpp \
../src/darling-dmg/src/DMGPartition.cpp \
../src/darling-dmg/src/EncryptReader.cpp \
../src/darling-dmg/src/FileReader.cpp \
../src/darling-dmg/src/GPTDisk.cpp \
../src/darling-dmg/src/HFSAttributeBTree.cpp \
../src/darling-dmg/src/HFSBTree.cpp \
../src/darling-dmg/src/HFSCatalogBTree.cpp \
../src/darling-dmg/src/HFSExtentsOverflowBTree.cpp \
../src/darling-dmg/src/HFSFork.cpp \
../src/darling-dmg/src/HFSHighLevelVolume.cpp \
../src/darling-dmg/src/HFSVolume.cpp \
../src/darling-dmg/src/HFSZlibReader.cpp \
../src/darling-dmg/src/MacBinary.cpp \
../src/darling-dmg/src/MemoryReader.cpp \
../src/darling-dmg/src/Reader.cpp \
../src/darling-dmg/src/ResourceFork.cpp \
../src/darling-dmg/src/SubReader.cpp \
../src/darling-dmg/src/adc.cpp 

OBJS += \
./src/darling-dmg/src/AppleDisk.o \
./src/darling-dmg/src/CacheZone.o \
./src/darling-dmg/src/CachedReader.o \
./src/darling-dmg/src/DMGDecompressor.o \
./src/darling-dmg/src/DMGDisk.o \
./src/darling-dmg/src/DMGPartition.o \
./src/darling-dmg/src/EncryptReader.o \
./src/darling-dmg/src/FileReader.o \
./src/darling-dmg/src/GPTDisk.o \
./src/darling-dmg/src/HFSAttributeBTree.o \
./src/darling-dmg/src/HFSBTree.o \
./src/darling-dmg/src/HFSCatalogBTree.o \
./src/darling-dmg/src/HFSExtentsOverflowBTree.o \
./src/darling-dmg/src/HFSFork.o \
./src/darling-dmg/src/HFSHighLevelVolume.o \
./src/darling-dmg/src/HFSVolume.o \
./src/darling-dmg/src/HFSZlibReader.o \
./src/darling-dmg/src/MacBinary.o \
./src/darling-dmg/src/MemoryReader.o \
./src/darling-dmg/src/Reader.o \
./src/darling-dmg/src/ResourceFork.o \
./src/darling-dmg/src/SubReader.o \
./src/darling-dmg/src/adc.o 

CPP_DEPS += \
./src/darling-dmg/src/AppleDisk.d \
./src/darling-dmg/src/CacheZone.d \
./src/darling-dmg/src/CachedReader.d \
./src/darling-dmg/src/DMGDecompressor.d \
./src/darling-dmg/src/DMGDisk.d \
./src/darling-dmg/src/DMGPartition.d \
./src/darling-dmg/src/EncryptReader.d \
./src/darling-dmg/src/FileReader.d \
./src/darling-dmg/src/GPTDisk.d \
./src/darling-dmg/src/HFSAttributeBTree.d \
./src/darling-dmg/src/HFSBTree.d \
./src/darling-dmg/src/HFSCatalogBTree.d \
./src/darling-dmg/src/HFSExtentsOverflowBTree.d \
./src/darling-dmg/src/HFSFork.d \
./src/darling-dmg/src/HFSHighLevelVolume.d \
./src/darling-dmg/src/HFSVolume.d \
./src/darling-dmg/src/HFSZlibReader.d \
./src/darling-dmg/src/MacBinary.d \
./src/darling-dmg/src/MemoryReader.d \
./src/darling-dmg/src/Reader.d \
./src/darling-dmg/src/ResourceFork.d \
./src/darling-dmg/src/SubReader.d \
./src/darling-dmg/src/adc.d 


# Each subdirectory must supply rules for building sources it contributes
src/darling-dmg/src/%.o: ../src/darling-dmg/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


