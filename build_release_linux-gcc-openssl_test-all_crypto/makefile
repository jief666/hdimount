################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src_tests_encryption/cryptoplusplus/subdir.mk
-include src_tests_encryption/crypto_openssl/subdir.mk
-include src_tests_encryption/crypto/subdir.mk
-include src_tests_encryption/subdir.mk
-include src/crypto/gladman-sha/subdir.mk
-include src/crypto/gladman-aes-modes/subdir.mk
-include src/crypto/gladman-aes/subdir.mk
-include src/crypto++/subdir.mk
-include src/crypto/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(M_DEPS)),)
-include $(M_DEPS)
endif
ifneq ($(strip $(MPP_DEPS)),)
-include $(MPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: test-all_crypto

# Tool invocations
test-all_crypto: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	linux-ld  -o "test-all_crypto" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(M_DEPS)$(MPP_DEPS)$(C_DEPS)$(CPP_DEPS) test-all_crypto
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
