################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CEpollMng.cpp \
../src/CUser.cpp \
../src/CUserMng.cpp \
../src/CircularBuf.cpp \
../src/Echo_Server_modify.cpp 

OBJS += \
./src/CEpollMng.o \
./src/CUser.o \
./src/CUserMng.o \
./src/CircularBuf.o \
./src/Echo_Server_modify.o 

CPP_DEPS += \
./src/CEpollMng.d \
./src/CUser.d \
./src/CUserMng.d \
./src/CircularBuf.d \
./src/Echo_Server_modify.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


