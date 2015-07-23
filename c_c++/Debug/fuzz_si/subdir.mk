################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fuzz_si/fuzzy01.c 

OBJS += \
./fuzz_si/fuzzy01.o 

C_DEPS += \
./fuzz_si/fuzzy01.d 


# Each subdirectory must supply rules for building sources it contributes
fuzz_si/%.o: ../fuzz_si/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


