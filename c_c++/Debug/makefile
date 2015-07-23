################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include rbfnn_test/Debug/subdir.mk
-include rbfnn_test/subdir.mk
-include rbfnn_pid_test/Debug/subdir.mk
-include rbfnn_pid_test/subdir.mk
-include pid_test/Debug/subdir.mk
-include pid_test/subdir.mk
-include fuzz_si/Debug/subdir.mk
-include fuzz_si/subdir.mk
-include desktop_controller/Debug/subdir.mk
-include desktop_controller/subdir.mk
-include NN_back1/Debug/subdir.mk
-include NN_back1/subdir.mk
-include Linear_regression/Debug/subdir.mk
-include Linear_regression/subdir.mk
-include LIB/RBFNN_PID/Debug/subdir.mk
-include LIB/RBFNN_PID/subdir.mk
-include LIB/RBFNN/Debug/subdir.mk
-include LIB/RBFNN/subdir.mk
-include LIB/PID/Debug/subdir.mk
-include LIB/PID/subdir.mk
-include Fuzzy_twoIn/Debug/subdir.mk
-include Fuzzy_twoIn/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ML_Linear_Regression

# Tool invocations
ML_Linear_Regression: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "ML_Linear_Regression" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) ML_Linear_Regression
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
