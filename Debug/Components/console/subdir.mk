################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Components/console/controller.cpp \
../Components/console/driver.cpp \
../Components/console/hal.cpp \
../Components/console/plot.cpp 

OBJS += \
./Components/console/controller.o \
./Components/console/driver.o \
./Components/console/hal.o \
./Components/console/plot.o 

CPP_DEPS += \
./Components/console/controller.d \
./Components/console/driver.d \
./Components/console/hal.d \
./Components/console/plot.d 


# Each subdirectory must supply rules for building sources it contributes
Components/console/%.o: ../Components/console/%.cpp Components/console/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/thanh.nguyen16/Documents/ThanhNguyen/mStack/Components" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

