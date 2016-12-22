################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Generated_Code/Cpu.c \
../Generated_Code/DbgCs1.c \
../Generated_Code/adConv1.c \
../Generated_Code/clockMan1.c \
../Generated_Code/flash1.c \
../Generated_Code/gpio1.c \
../Generated_Code/hardware_init.c \
../Generated_Code/i2cCom1.c \
../Generated_Code/lpTmr1.c \
../Generated_Code/osa1.c \
../Generated_Code/pin_init.c \
../Generated_Code/pwrMan1.c \
../Generated_Code/uartCom1.c 

OBJS += \
./Generated_Code/Cpu.o \
./Generated_Code/DbgCs1.o \
./Generated_Code/adConv1.o \
./Generated_Code/clockMan1.o \
./Generated_Code/flash1.o \
./Generated_Code/gpio1.o \
./Generated_Code/hardware_init.o \
./Generated_Code/i2cCom1.o \
./Generated_Code/lpTmr1.o \
./Generated_Code/osa1.o \
./Generated_Code/pin_init.o \
./Generated_Code/pwrMan1.o \
./Generated_Code/uartCom1.o 

C_DEPS += \
./Generated_Code/Cpu.d \
./Generated_Code/DbgCs1.d \
./Generated_Code/adConv1.d \
./Generated_Code/clockMan1.d \
./Generated_Code/flash1.d \
./Generated_Code/gpio1.d \
./Generated_Code/hardware_init.d \
./Generated_Code/i2cCom1.d \
./Generated_Code/lpTmr1.d \
./Generated_Code/osa1.d \
./Generated_Code/pin_init.d \
./Generated_Code/pwrMan1.d \
./Generated_Code/uartCom1.d 


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/%.o: ../Generated_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"FSL_OSA_BM_TIMER_CONFIG=2" -D"CPU_MKL26Z128VLH4" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/hal/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/hal/src/sim/MKL26Z4" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/system/src/clock/MKL26Z4" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/system/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/osa/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/CMSIS/Include" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/devices" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/devices/MKL26Z4/include" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/utilities/src" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/utilities/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/devices/MKL26Z4/startup" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/Generated_Code/SDK/platform/devices/MKL26Z4/startup" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/Sources" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/Generated_Code" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/drivers/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/system/src/power" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/drivers/src/flash/C90TFS/drvsrc/include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


