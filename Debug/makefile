################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := ${ComSpec}

-include ../makefile.init

RM := clean

# All of the sources participating in the build are defined here
-include sources.mk
-include system/uCOS-III/TLS/CCES/subdir.mk
-include system/uCOS-III/Source/subdir.mk
-include system/uCOS-III/Ports/Blackfin/subdir.mk
-include system/uCOS-III/GeneratedSources/subdir.mk
-include system/uCOS-III/AppSources/subdir.mk
-include system/uC-LIB/Source/subdir.mk
-include system/uC-Common/AppSources/subdir.mk
-include system/uC-CPU/Source/subdir.mk
-include system/uC-CPU/Ports/Blackfin/CCES/subdir.mk
-include system/startup_ldf/subdir.mk
-include system/services/wd/subdir.mk
-include system/services/tru/subdir.mk
-include system/services/tmr/subdir.mk
-include system/services/stdio/subdir.mk
-include system/services/pwr/subdir.mk
-include system/services/pwm/subdir.mk
-include system/services/gpio/subdir.mk
-include system/services/dma/subdir.mk
-include system/services/acm/subdir.mk
-include system/pinmux/GeneratedSources/subdir.mk
-include system/mcapi/GeneratedSources/subdir.mk
-include system/drivers/vss/subdir.mk
-include system/drivers/usb/controller/musbmhdrc/subdir.mk
-include system/drivers/uart/subdir.mk
-include system/drivers/twi/subdir.mk
-include system/drivers/sport/subdir.mk
-include system/drivers/spi/subdir.mk
-include system/drivers/rsi/subdir.mk
-include system/drivers/rotary/subdir.mk
-include system/drivers/pvp/subdir.mk
-include system/drivers/ppi/subdir.mk
-include system/drivers/pixc/subdir.mk
-include system/drivers/linkport/subdir.mk
-include system/drivers/ethernet/gemac/subdir.mk
-include system/drivers/ethernet/subdir.mk
-include system/drivers/crc/subdir.mk
-include system/subdir.mk
-include src/subdir.mk
-include soft_update/Flash/subdir.mk
-include soft_update/subdir.mk
-include XL902_COMMUNICATION/subdir.mk
-include XL902RippleWave/subdir.mk
-include XL902PowerManager/subdir.mk
-include UARTx/subdir.mk
-include TIMER/subdir.mk
-include SWITCH/subdir.mk
-include PIN_INI/subdir.mk
-include LTC2943/subdir.mk
-include INIT_MEMORY/subdir.mk
-include EnergyPulse/subdir.mk
-include DataStruct/subdir.mk
-include Charger-BMS_Communication/subdir.mk
-include CGU/subdir.mk
-include CAN/subdir.mk
-include AD7793_SPI/subdir.mk
-include AD7608/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(DSP_DEPS)),)
-include $(DSP_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
DXE += \
charging-checkV0_0_Core0.dxe \

LINKER_LOG_OUTPUTS += \
linker_log.xml \


# All Target
all: charging-checkV0_0_Core0.ldr

# Tool invocations
charging-checkV0_0_Core0.ldr: $(DXE)
	@echo 'Building target: $@'
	@echo 'Invoking: CrossCore Blackfin Loader'
	elfloader.exe -proc ADSP-BF609 -si-revision 0.2 -b MEMORY -f hex -width 16 -bcode 0x06 -init C:/Analog\ Devices/CrossCore\ Embedded\ Studio\ 1.1.1/Blackfin/ldr/BF609_init_v01.dxe -o charging-checkV0_0_Core0.ldr $(DXE)
	@echo 'Finished building target: $@'
	@echo ' '

charging-checkV0_0_Core0.dxe: $(OBJS) $(LDF_SRCS)
	@echo 'Invoking: CrossCore Blackfin Linker'
	ccblkfn.exe -proc ADSP-BF609 -si-revision 0.2 -component C:/Program\ Files\ (x86)/Common\ Files/Analog\ Devices/uC-LIB\ v1.1.0/libraries.xml -component C:/Analog\ Devices/uCOS-III-Rel1.1.0/uCOS-III/libraries.xml -TE:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/startup_ldf/app.ldf -no-mem -LE:/mydot/mydot/my_code/charging-checkV0_0_Core0/Debug/system/startup_ldf -flags-link -MDCORE0,-MDDEBUG,-MDADI_MCAPI,-MD_ADI_THREADS -flags-link -e -add-debug-libpaths -threads -flags-link -MDUSE_SDRAM -flags-link -keep,_OSDbg_SchedRoundRobinEn, -flags-link -keep,_OSDbg_VersionNbr, -flags-link -keep,_OSFlagDbgListPtr, -flags-link -keep,_OSMemDbgListPtr, -flags-link -keep,_OSMutexDbgListPtr, -flags-link -keep,_OSQDbgListPtr, -flags-link -keep,_OSRunning, -flags-link -keep,_OSSchedLockNestingCtr, -flags-link -keep,_OSSemDbgListPtr, -flags-link -keep,_OSTaskDbgListPtr, -flags-link -keep,_OSTickCtr, -flags-link libosal_ucos3.dlb -o charging-checkV0_0_Core0.dxe $(OBJS) $(LDF_SRCS)  -flags-link -od,.\.
	@echo 'Finished building: $@'
	@echo ' '

linker_log.xml: charging-checkV0_0_Core0.dxe $(OBJS) $(LDF_SRCS)

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(LOADER_FILE_OUTPUTS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(DSP_DEPS)$(DXE)$(CPP_DEPS)$(LINKER_LOG_OUTPUTS)$(S_DEPS)$(CXX_DEPS) charging-checkV0_0_Core0.ldr
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
