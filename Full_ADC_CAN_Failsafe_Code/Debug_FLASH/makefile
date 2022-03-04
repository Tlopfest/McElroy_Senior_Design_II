################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include include/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
Full_ADC_CAN_Failsafe_Code.siz \


# All Target
all: Full_ADC_CAN_Failsafe_Code.elf secondary-outputs

# Tool invocations
Full_ADC_CAN_Failsafe_Code.elf: $(OBJS) /home/tlopfest/Documents/S32_Workspace/Full_ADC_CAN_Failsafe_Code/Project_Settings/Linker_Files/S32K144_64_flash.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "Full_ADC_CAN_Failsafe_Code.elf" "@Full_ADC_CAN_Failsafe_Code.args"  $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Full_ADC_CAN_Failsafe_Code.siz: Full_ADC_CAN_Failsafe_Code.elf
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley Full_ADC_CAN_Failsafe_Code.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
