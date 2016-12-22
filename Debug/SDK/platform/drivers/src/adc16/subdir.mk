################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/platform/drivers/src/adc16/fsl_adc16_common.c \
../SDK/platform/drivers/src/adc16/fsl_adc16_driver.c 

OBJS += \
./SDK/platform/drivers/src/adc16/fsl_adc16_common.o \
./SDK/platform/drivers/src/adc16/fsl_adc16_driver.o 

C_DEPS += \
./SDK/platform/drivers/src/adc16/fsl_adc16_common.d \
./SDK/platform/drivers/src/adc16/fsl_adc16_driver.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/drivers/src/adc16/%.o: ../SDK/platform/drivers/src/adc16/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"FSL_OSA_BM_TIMER_CONFIG=2" -D"CPU_MKL26Z128VLH4" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/hal/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/hal/src/sim/MKL26Z4" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/system/src/clock/MKL26Z4" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/system/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/osa/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/CMSIS/Include" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/devices" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/devices/MKL26Z4/include" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/utilities/src" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/utilities/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/devices/MKL26Z4/startup" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/Generated_Code/SDK/platform/devices/MKL26Z4/startup" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/Sources" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/Generated_Code" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/drivers/inc" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/system/src/power" -I"C:/Lavoro/Progetti/RSR/RSR1083/prj_lora/Lora/SDK/platform/drivers/src/flash/C90TFS/drvsrc/include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


