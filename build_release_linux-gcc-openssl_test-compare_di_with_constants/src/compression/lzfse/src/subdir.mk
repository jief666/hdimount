################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/compression/lzfse/src/lzfse_decode.c \
../src/compression/lzfse/src/lzfse_decode_base.c \
../src/compression/lzfse/src/lzfse_encode.c \
../src/compression/lzfse/src/lzfse_encode_base.c \
../src/compression/lzfse/src/lzfse_fse.c \
../src/compression/lzfse/src/lzvn_decode_base.c \
../src/compression/lzfse/src/lzvn_encode_base.c 

OBJS += \
./src/compression/lzfse/src/lzfse_decode.o \
./src/compression/lzfse/src/lzfse_decode_base.o \
./src/compression/lzfse/src/lzfse_encode.o \
./src/compression/lzfse/src/lzfse_encode_base.o \
./src/compression/lzfse/src/lzfse_fse.o \
./src/compression/lzfse/src/lzvn_decode_base.o \
./src/compression/lzfse/src/lzvn_encode_base.o 

C_DEPS += \
./src/compression/lzfse/src/lzfse_decode.d \
./src/compression/lzfse/src/lzfse_decode_base.d \
./src/compression/lzfse/src/lzfse_encode.d \
./src/compression/lzfse/src/lzfse_encode_base.d \
./src/compression/lzfse/src/lzfse_fse.d \
./src/compression/lzfse/src/lzvn_decode_base.d \
./src/compression/lzfse/src/lzvn_encode_base.d 


# Each subdirectory must supply rules for building sources it contributes
src/compression/lzfse/src/%.o: ../src/compression/lzfse/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	linux-gcc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


