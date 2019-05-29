################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/apfs-fuse/ApfsLib/ApfsContainer.cpp \
../src/apfs-fuse/ApfsLib/ApfsDir.cpp \
../src/apfs-fuse/ApfsLib/ApfsNodeMapper.cpp \
../src/apfs-fuse/ApfsLib/ApfsNodeMapperBTree.cpp \
../src/apfs-fuse/ApfsLib/ApfsVolume.cpp \
../src/apfs-fuse/ApfsLib/BTree.cpp \
../src/apfs-fuse/ApfsLib/BlockDumper.cpp \
../src/apfs-fuse/ApfsLib/CheckPointMap.cpp \
../src/apfs-fuse/ApfsLib/Decmpfs.cpp \
../src/apfs-fuse/ApfsLib/KeyMgmt.cpp \
../src/apfs-fuse/ApfsLib/Unicode.cpp \
../src/apfs-fuse/ApfsLib/Util.cpp 

OBJS += \
./src/apfs-fuse/ApfsLib/ApfsContainer.o \
./src/apfs-fuse/ApfsLib/ApfsDir.o \
./src/apfs-fuse/ApfsLib/ApfsNodeMapper.o \
./src/apfs-fuse/ApfsLib/ApfsNodeMapperBTree.o \
./src/apfs-fuse/ApfsLib/ApfsVolume.o \
./src/apfs-fuse/ApfsLib/BTree.o \
./src/apfs-fuse/ApfsLib/BlockDumper.o \
./src/apfs-fuse/ApfsLib/CheckPointMap.o \
./src/apfs-fuse/ApfsLib/Decmpfs.o \
./src/apfs-fuse/ApfsLib/KeyMgmt.o \
./src/apfs-fuse/ApfsLib/Unicode.o \
./src/apfs-fuse/ApfsLib/Util.o 

CPP_DEPS += \
./src/apfs-fuse/ApfsLib/ApfsContainer.d \
./src/apfs-fuse/ApfsLib/ApfsDir.d \
./src/apfs-fuse/ApfsLib/ApfsNodeMapper.d \
./src/apfs-fuse/ApfsLib/ApfsNodeMapperBTree.d \
./src/apfs-fuse/ApfsLib/ApfsVolume.d \
./src/apfs-fuse/ApfsLib/BTree.d \
./src/apfs-fuse/ApfsLib/BlockDumper.d \
./src/apfs-fuse/ApfsLib/CheckPointMap.d \
./src/apfs-fuse/ApfsLib/Decmpfs.d \
./src/apfs-fuse/ApfsLib/KeyMgmt.d \
./src/apfs-fuse/ApfsLib/Unicode.d \
./src/apfs-fuse/ApfsLib/Util.d 


# Each subdirectory must supply rules for building sources it contributes
src/apfs-fuse/ApfsLib/%.o: ../src/apfs-fuse/ApfsLib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


