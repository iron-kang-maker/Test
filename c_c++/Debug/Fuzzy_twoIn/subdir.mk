################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Fuzzy_twoIn/fuzz02.c 

OBJS += \
./Fuzzy_twoIn/fuzz02.o 

C_DEPS += \
./Fuzzy_twoIn/fuzz02.d 


# Each subdirectory must supply rules for building sources it contributes
Fuzzy_twoIn/%.o: ../Fuzzy_twoIn/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


