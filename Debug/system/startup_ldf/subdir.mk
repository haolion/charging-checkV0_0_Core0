################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/startup_ldf/app_cplbtab.c \
../system/startup_ldf/app_handler_table.c \
../system/startup_ldf/app_heaptab.c 

S_SRCS += \
../system/startup_ldf/app_startup.s 

LDF_SRCS += \
../system/startup_ldf/app.ldf 

OBJS += \
./system/startup_ldf/app_cplbtab.doj \
./system/startup_ldf/app_handler_table.doj \
./system/startup_ldf/app_heaptab.doj \
./system/startup_ldf/app_startup.doj 

C_DEPS += \
./system/startup_ldf/app_cplbtab.d \
./system/startup_ldf/app_handler_table.d \
./system/startup_ldf/app_heaptab.d 

S_DEPS += \
./system/startup_ldf/app_startup.d 


# Each subdirectory must supply rules for building sources it contributes
system/startup_ldf/%.doj: ../system/startup_ldf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore Blackfin C/C++ Compiler'
	ccblkfn.exe -c -file-attr ProjectName="charging-checkV0_0_Core0" -proc ADSP-BF609 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG -DADI_MCAPI -DADI_UCOS3_PRODUCT -DADI_USE_UCOS3_SOURCE -I"E:\mydot\mydot\my_code\charging-checkV0_0_Core0\system" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-Common/GeneratedSources" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-CPU/GeneratedSources" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-CPU v1.1.0/common/uC-CPU" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-LIB/GeneratedSources" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-LIB v1.1.0/common/uC-LIB" -I"C:/Program Files (x86)/Common Files/Analog Devices/uC-CPU v1.1.0/Blackfin/include" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/Blackfin/uCOS-III/Ports" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uCOS-III/Cfg" -I"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uCOS-III/GeneratedSources" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/common/uCOS-III" -I"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/common/uCOS-III/Source" -structs-do-not-overlap -no-multiline -warn-protos -threads -double-size-32 -decls-strong -cplbs -gnu-style-dependencies -MD -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/startup_ldf/%.doj: ../system/startup_ldf/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore Blackfin Assembler'
	easmblkfn.exe -file-attr ProjectName="charging-checkV0_0_Core0" -proc ADSP-BF609 -si-revision 0.2 -D_ADI_THREADS -g -DCORE0 -D_DEBUG -DADI_MCAPI -DADI_UCOS3_PRODUCT -DADI_USE_UCOS3_SOURCE -i"E:\mydot\mydot\my_code\charging-checkV0_0_Core0\system" -i"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-Common/GeneratedSources" -i"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-CPU/GeneratedSources" -i"C:/Program Files (x86)/Common Files/Analog Devices/uC-CPU v1.1.0/common/uC-CPU" -i"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uC-LIB/GeneratedSources" -i"C:/Program Files (x86)/Common Files/Analog Devices/uC-LIB v1.1.0/common/uC-LIB" -i"C:/Program Files (x86)/Common Files/Analog Devices/uC-CPU v1.1.0/Blackfin/include" -i"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/Blackfin/uCOS-III/Ports" -i"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uCOS-III/Cfg" -i"E:/mydot/mydot/my_code/charging-checkV0_0_Core0/system/uCOS-III/GeneratedSources" -i"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/common/uCOS-III" -i"C:/Analog Devices/uCOS-III-Rel1.1.0/uCOS-III/common/uCOS-III/Source" -gnu-style-dependencies -MM -Mo "$(basename $@).d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


