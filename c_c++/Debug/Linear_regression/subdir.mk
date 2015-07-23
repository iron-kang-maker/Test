################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Linear_regression/linear_regression.c 

OBJS += \
./Linear_regression/linear_regression.o 

C_DEPS += \
./Linear_regression/linear_regression.d 


# Each subdirectory must supply rules for building sources it contributes
Linear_regression/%.o: ../Linear_regression/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


