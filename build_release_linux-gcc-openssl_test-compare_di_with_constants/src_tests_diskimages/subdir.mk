################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src_tests_diskimages/DiskImage.cpp \
../src_tests_diskimages/DiskImageKind.cpp \
../src_tests_diskimages/DiskImageTestUtils.cpp \
../src_tests_diskimages/DiskImageToTest.cpp \
../src_tests_diskimages/FolderReferenceCreation.cpp \
../src_tests_diskimages/PrintBuf.cpp \
../src_tests_diskimages/io.cpp 

OBJS += \
./src_tests_diskimages/DiskImage.o \
./src_tests_diskimages/DiskImageKind.o \
./src_tests_diskimages/DiskImageTestUtils.o \
./src_tests_diskimages/DiskImageToTest.o \
./src_tests_diskimages/FolderReferenceCreation.o \
./src_tests_diskimages/PrintBuf.o \
./src_tests_diskimages/io.o 

CPP_DEPS += \
./src_tests_diskimages/DiskImage.d \
./src_tests_diskimages/DiskImageKind.d \
./src_tests_diskimages/DiskImageTestUtils.d \
./src_tests_diskimages/DiskImageToTest.d \
./src_tests_diskimages/FolderReferenceCreation.d \
./src_tests_diskimages/PrintBuf.d \
./src_tests_diskimages/io.d 


# Each subdirectory must supply rules for building sources it contributes
src_tests_diskimages/%.o: ../src_tests_diskimages/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	linux-g++ -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


