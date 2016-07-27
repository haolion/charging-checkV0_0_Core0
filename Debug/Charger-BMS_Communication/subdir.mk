################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Charger-BMS_Communication/CAN2.0.c \
../Charger-BMS_Communication/CBMS_APLayer.c \
../Charger-BMS_Communication/SAE_J1939.c 

OBJS += \
./Charger-BMS_Communication/CAN2.0.doj \
./Charger-BMS_Communication/CBMS_APLayer.doj \
./Charger-BMS_Communication/SAE_J1939.doj 

C_DEPS += \
./Charger-BMS_Communication/CAN2.0.d \
./Charger-BMS_Communication/CBMS_APLayer.d \
./Charger-BMS_Communication/SAE_J1939.d 


# Each subdirectory must supply rules for building sources it contributes
Charger-BMS_Communication/%.doj: ../Charger-BMS_Communication/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore Blackfin C/C++ Compiler'
	ccblkfn.exe -c -file-attr ProjectName="charging-checkV0_0_Core0" -proc ADSP-BF609 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG -DADI_MCAPI -DADI_UCOS3_PRODUCT -DADI_USE_UCOS3_SOURCE -I"E:\mydot\mydot\my_code\charging-checkV0_0_Core0\system" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-Common/GeneratedSources" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-CPU/GeneratedSources" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-CPU v1.1.0/common/uC-CPU" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-LIB/GeneratedSources" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-LIB v1.1.0/common/uC-LIB" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-CPU v1.1.0/Blackfin/include" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/Blackfin/uCOS-III/Ports" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uCOS-III/Cfg" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uCOS-III/GeneratedSources" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/common/uCOS-III" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/common/uCOS-III/Source" -structs-do-not-overlap -no-multiline -warn-protos -threads -double-size-32 -decls-strong -cplbs -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


