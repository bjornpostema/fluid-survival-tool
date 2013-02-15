################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/trunk/DFPN2.cpp \
../src/trunk/Line.cpp \
../src/trunk/Region.cpp \
../src/trunk/TimedDiagram.cpp \
../src/trunk/main.cpp 

OBJS += \
./src/trunk/DFPN2.o \
./src/trunk/Line.o \
./src/trunk/Region.o \
./src/trunk/TimedDiagram.o \
./src/trunk/main.o 

CPP_DEPS += \
./src/trunk/DFPN2.d \
./src/trunk/Line.d \
./src/trunk/Region.d \
./src/trunk/TimedDiagram.d \
./src/trunk/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/trunk/%.o: ../src/trunk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


