################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rbfnn_test/rbfnn_test.c 

OBJS += \
./rbfnn_test/rbfnn_test.o 

C_DEPS += \
./rbfnn_test/rbfnn_test.d 


# Each subdirectory must supply rules for building sources it contributes
rbfnn_test/%.o: ../rbfnn_test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


