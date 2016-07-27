################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../XL902_COMMUNICATION/XL902_Communication.c 

OBJS += \
./XL902_COMMUNICATION/XL902_Communication.doj 

C_DEPS += \
./XL902_COMMUNICATION/XL902_Communication.d 


# Each subdirectory must supply rules for building sources it contributes
XL902_COMMUNICATION/%.doj: ../XL902_COMMUNICATION/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore Blackfin C/C++ Compiler'
	ccblkfn.exe -c -file-attr ProjectName="charging-checkV0_0_Core0" -proc ADSP-BF609 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG -DADI_MCAPI -DADI_UCOS3_PRODUCT -DADI_USE_UCOS3_SOURCE -I"E:\mydot\mydot\my_code\charging-checkV0_0_Core0\system" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-Common/GeneratedSources" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-CPU/GeneratedSources" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-CPU v1.1.0/common/uC-CPU" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-LIB/GeneratedSources" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-LIB v1.1.0/common/uC-LIB" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-CPU v1.1.0/Blackfin/include" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/Blackfin/uCOS-III/Ports" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uCOS-III/Cfg" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uCOS-III/GeneratedSources" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/common/uCOS-III" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/common/uCOS-III/Source" -structs-do-not-overlap -no-multiline -warn-protos -threads -double-size-32 -decls-strong -cplbs -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


