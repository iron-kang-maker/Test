################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LIB/RBFNN/rbfnn.c 

OBJS += \
./LIB/RBFNN/rbfnn.o 

C_DEPS += \
./LIB/RBFNN/rbfnn.d 


# Each subdirectory must supply rules for building sources it contributes
LIB/RBFNN/%.o: ../LIB/RBFNN/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


