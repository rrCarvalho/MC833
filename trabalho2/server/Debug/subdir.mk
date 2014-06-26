################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../auxf.c \
../control.c \
../server.c \
../state.c \
../wrapsock.c 

OBJS += \
./auxf.o \
./control.o \
./server.o \
./state.o \
./wrapsock.o 

C_DEPS += \
./auxf.d \
./control.d \
./server.d \
./state.d \
./wrapsock.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


