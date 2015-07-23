################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../NN_back1/back1.c 

OBJS += \
./NN_back1/back1.o 

C_DEPS += \
./NN_back1/back1.d 


# Each subdirectory must supply rules for building sources it contributes
NN_back1/%.o: ../NN_back1/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


