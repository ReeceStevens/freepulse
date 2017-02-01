# Name of the binaries.
PROJ_NAME=freepulse

######################################################################
#                         SETUP SOURCES                              #
######################################################################


# This is the directory containing the firmware package
STM_DIR=/home/reece/projects/freepulse/tools/STM32F4-Discovery_FW_V1.1.0
PERIPH_DRIVERS_DIR = /home/reece/projects/freepulse/tools/STM32F4_STANDARD_PERIPHERAL_DRIVERS

# This is where the source files are located,
# which are not in the current directory
# SRC_DIRS = $(STM_DIR)/Libraries/STM32F4xx_StdPeriph_Driver/src
SRC_DIRS = $(PERIPH_DRIVERS_DIR)/STM32F4xx_StdPeriph_Driver_working/src
SRC_DIRS += $(PERIPH_DRIVERS_DIR)/CMSIS/Device/ST/STM32F4xx/src
SRC_DIRS += ./libraries/src
SRC_DIRS += ./system/src 
SRC_DIRS += ./src

# Tell make to look in that folder if it cannot find a source
# in the current directory
vpath %.c $(SRC_DIRS)
vpath %.cpp $(SRC_DIRS)

SRCS   = main.cpp
SRCS  += system_stm32f4xx.c
SRCS  += stm32f4xx_rcc.c 
SRCS  += stm32f4xx_gpio.c
SRCS  += stm32f4xx_spi.c
SRCS  += stm32f4xx_adc.c
SRCS  += stm32f4xx_tim.c
SRCS  += stm32f4xx_syscfg.c
SRCS  += stm32f4xx_usart.c
SRCS  += stm32f4xx_exti.c
SRCS  += misc.c
SRCS  += Adafruit_RA8875.cpp
SRCS  += Adafruit_GFX.cpp
SRCS  += startup.s

INC_DIRS  = $(PERIPH_DRIVERS_DIR)/STM32F4xx_StdPeriph_Driver_working/inc
INC_DIRS += $(PERIPH_DRIVERS_DIR)/CMSIS/Device/ST/STM32F4xx/Include
INC_DIRS += $(PERIPH_DRIVERS_DIR)/CMSIS/Include
INC_DIRS += $(PERIPH_DRIVERS_DIR)/CMSIS/ST/STM32F4xx/Include
INC_DIRS += ./libraries/inc
INC_DIRS += ./system/inc
INC_DIRS += ./inc

GTEST_ROOT = /home/reece/projects/freepulse/googletest/googletest
TEST_ROOT  = ./tests
TEST_DIRS  = $(INC_DIRS)
TEST_DIRS += $(GTEST_ROOT)/include
TEST_SRCS  = $(TEST_ROOT)/CircleBufferTest.cpp
TEST_LIBS  = $(GTEST_ROOT)/make/gtest-all.o

######################################################################
#                         SETUP TOOLS                                #
######################################################################

# This is the path to the toolchain
CC      = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
GDB     = arm-none-eabi-gdb
TESTCC  = g++ 

STLINK = /home/reece/projects/freepulse/tools/stlink/build/st-flash

INCLUDE = $(addprefix -I,$(INC_DIRS))
TESTINCS = $(addprefix -I,$(TEST_DIRS))

# #defines needed when working with the STM library
DEFS    = -DUSE_STDPERIPH_DRIVER -DSTM32F411xE

## Compiler options
CFLAGS  = -ggdb
CFLAGS += -O0 
CFLAGS += -Wall -Wextra -Warray-bounds
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -felide-constructors -std=c++0x

## Linker options
LD_DIR = ./linker
LFLAGS  = -T$(LD_DIR)/stm32_flash.ld -T$(LD_DIR)/libs.ld

######################################################################
#                         SETUP TARGETS                              #
######################################################################

.PHONY: $(PROJ_NAME)
$(PROJ_NAME): $(PROJ_NAME).elf

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(INCLUDE) $(DEFS) $(CFLAGS) $(LFLAGS) $^ -o $@ 
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf   $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin test

# Flash the STM32F4
flash: 
	st-flash write $(PROJ_NAME).bin 0x8000000

# Flash via UART (v1.2 Prototype)
uart-flash:
	stm32flash -b 115200 -w $(PROJ_NAME).bin -v -g 0x0 /dev/ttyUSB0

.PHONY: debug
debug:
# before you start gdb, you must start st-util
# /home/reece/projects/freepulse/tools/stlink/build/src/gdbserver/st-util[
	arm-none-eabi-gdb $(PROJ_NAME).elf

test: $(TEST_SRCS)
	$(TESTCC) $(TESTINCS) -lpthread $(TEST_LIBS) $^ -o $@
	./$@
