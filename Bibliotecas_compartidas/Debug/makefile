################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include server_multithread/subdir.mk
-include server/subdir.mk
-include serializacion/subdir.mk
-include random/subdir.mk
-include parser/subdir.mk
-include hash/subdir.mk
-include epoch/subdir.mk
-include cliente/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libBibliotecas_compartidas.so

# Tool invocations
libBibliotecas_compartidas.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -shared -o "libBibliotecas_compartidas.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(OBJS)$(C_DEPS) libBibliotecas_compartidas.so
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
