################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../desktop_controller/desktop_controller.c 

OBJS += \
./desktop_controller/desktop_controller.o 

C_DEPS += \
./desktop_controller/desktop_controller.d 


# Each subdirectory must supply rules for building sources it contributes
desktop_controller/%.o: ../desktop_controller/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


