################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LIB/RBFNN_PID/rbfnn_pid.c 

OBJS += \
./LIB/RBFNN_PID/rbfnn_pid.o 

C_DEPS += \
./LIB/RBFNN_PID/rbfnn_pid.d 


# Each subdirectory must supply rules for building sources it contributes
LIB/RBFNN_PID/%.o: ../LIB/RBFNN_PID/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


