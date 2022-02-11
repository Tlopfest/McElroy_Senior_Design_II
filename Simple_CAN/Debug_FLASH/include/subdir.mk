################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../include/FlexCAN.c \
../include/clocks_and_modes.c 

OBJS += \
./include/FlexCAN.o \
./include/clocks_and_modes.o 

C_DEPS += \
./include/FlexCAN.d \
./include/clocks_and_modes.d 


# Each subdirectory must supply rules for building sources it contributes
include/%.o: ../include/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@include/FlexCAN.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


