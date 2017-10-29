################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cp25Packing.cpp \
../src/packing.cpp 

OBJS += \
./src/cp25Packing.o \
./src/packing.o 

CPP_DEPS += \
./src/cp25Packing.d \
./src/packing.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DIL_STD -DNDEBUG -DILOUSEMT -D_REENTRANT -DILM_REENTRANT -I/opt/ILOG/CPLEX/cpoptimizer/include -I/opt/ibm/ILOG/CPLEX_Studio127/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio127/concert/include -O3 -pedantic -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


