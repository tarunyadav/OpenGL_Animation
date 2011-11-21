################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../imageloader.cpp \
../main.cpp \
../md2model.cpp \
../vec3f.cpp 

OBJS += \
./imageloader.o \
./main.o \
./md2model.o \
./vec3f.o 

CPP_DEPS += \
./imageloader.d \
./main.d \
./md2model.d \
./vec3f.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


