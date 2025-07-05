################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ADIS16475.c \
../Core/Src/comms_tasks.c \
../Core/Src/error_monitor_task.c \
../Core/Src/it.c \
../Core/Src/main.c \
../Core/Src/msp.c \
../Core/Src/sensor_task.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/telemetry_packet.c \
../Core/Src/temp_task.c \
../Core/Src/timebase_tim.c 

OBJS += \
./Core/Src/ADIS16475.o \
./Core/Src/comms_tasks.o \
./Core/Src/error_monitor_task.o \
./Core/Src/it.o \
./Core/Src/main.o \
./Core/Src/msp.o \
./Core/Src/sensor_task.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/telemetry_packet.o \
./Core/Src/temp_task.o \
./Core/Src/timebase_tim.o 

C_DEPS += \
./Core/Src/ADIS16475.d \
./Core/Src/comms_tasks.d \
./Core/Src/error_monitor_task.d \
./Core/Src/it.d \
./Core/Src/main.d \
./Core/Src/msp.d \
./Core/Src/sensor_task.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/telemetry_packet.d \
./Core/Src/temp_task.d \
./Core/Src/timebase_tim.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Excallibur/Documents/WorkSpace2/Rtos_workspace2/common/ThirdParty/FreeRTOS/include" -I"C:/Users/Excallibur/Documents/WorkSpace2/Rtos_workspace2/common/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/Excallibur/Documents/WorkSpace2/Rtos_workspace2/common/ThirdParty/FreeRTOS" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ADIS16475.cyclo ./Core/Src/ADIS16475.d ./Core/Src/ADIS16475.o ./Core/Src/ADIS16475.su ./Core/Src/comms_tasks.cyclo ./Core/Src/comms_tasks.d ./Core/Src/comms_tasks.o ./Core/Src/comms_tasks.su ./Core/Src/error_monitor_task.cyclo ./Core/Src/error_monitor_task.d ./Core/Src/error_monitor_task.o ./Core/Src/error_monitor_task.su ./Core/Src/it.cyclo ./Core/Src/it.d ./Core/Src/it.o ./Core/Src/it.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/msp.cyclo ./Core/Src/msp.d ./Core/Src/msp.o ./Core/Src/msp.su ./Core/Src/sensor_task.cyclo ./Core/Src/sensor_task.d ./Core/Src/sensor_task.o ./Core/Src/sensor_task.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/telemetry_packet.cyclo ./Core/Src/telemetry_packet.d ./Core/Src/telemetry_packet.o ./Core/Src/telemetry_packet.su ./Core/Src/temp_task.cyclo ./Core/Src/temp_task.d ./Core/Src/temp_task.o ./Core/Src/temp_task.su ./Core/Src/timebase_tim.cyclo ./Core/Src/timebase_tim.d ./Core/Src/timebase_tim.o ./Core/Src/timebase_tim.su

.PHONY: clean-Core-2f-Src

