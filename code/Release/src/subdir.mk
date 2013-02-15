################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DFPN2.cpp \
../src/Line.cpp \
../src/Region.cpp \
../src/TimedDiagram.cpp \
../src/main.cpp 

OBJS += \
./src/DFPN2.o \
./src/Line.o \
./src/Region.o \
./src/TimedDiagram.o \
./src/main.o 

CPP_DEPS += \
./src/DFPN2.d \
./src/Line.d \
./src/Region.d \
./src/TimedDiagram.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


