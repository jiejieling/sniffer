################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/handle_pack.c \
../src/hashtable.c \
../src/heapSort.c \
../src/mypcap.c \
../src/save_pack.c 

OBJS += \
./src/handle_pack.o \
./src/hashtable.o \
./src/heapSort.o \
./src/mypcap.o \
./src/save_pack.o 

C_DEPS += \
./src/handle_pack.d \
./src/hashtable.d \
./src/heapSort.d \
./src/mypcap.d \
./src/save_pack.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/pcap/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


