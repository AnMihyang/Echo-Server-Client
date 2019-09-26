################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CEpollMng.cpp \
../src/CUserMng.cpp \
../src/CircularBuffer.cpp \
../src/DataMng.cpp \
../src/Echo_server_ver1.cpp 

OBJS += \
./src/CEpollMng.o \
./src/CUserMng.o \
./src/CircularBuffer.o \
./src/DataMng.o \
./src/Echo_server_ver1.o 

CPP_DEPS += \
./src/CEpollMng.d \
./src/CUserMng.d \
./src/CircularBuffer.d \
./src/DataMng.d \
./src/Echo_server_ver1.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


