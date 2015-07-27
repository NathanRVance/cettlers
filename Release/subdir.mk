################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ai_playcard.c \
../ai_pregame.c \
../ai_robber.c \
../ai_routine.c \
../ai_trade.c \
../ai_util.c \
../animate.c \
../builder.c \
../data.c \
../devcards.c \
../game_setup.c \
../io.c \
../longestroad.c \
../main.c \
../map.c \
../marker.c \
../player_trade.c \
../road.c \
../robber.c \
../stats.c \
../trade.c \
../util.c 

OBJS += \
./ai_playcard.o \
./ai_pregame.o \
./ai_robber.o \
./ai_routine.o \
./ai_trade.o \
./ai_util.o \
./animate.o \
./builder.o \
./data.o \
./devcards.o \
./game_setup.o \
./io.o \
./longestroad.o \
./main.o \
./map.o \
./marker.o \
./player_trade.o \
./road.o \
./robber.o \
./stats.o \
./trade.o \
./util.o 

C_DEPS += \
./ai_playcard.d \
./ai_pregame.d \
./ai_robber.d \
./ai_routine.d \
./ai_trade.d \
./ai_util.d \
./animate.d \
./builder.d \
./data.d \
./devcards.d \
./game_setup.d \
./io.d \
./longestroad.d \
./main.d \
./map.d \
./marker.d \
./player_trade.d \
./road.d \
./robber.d \
./stats.d \
./trade.d \
./util.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


