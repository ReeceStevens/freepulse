# Name of the binaries.
PROJ_NAME=freepulse

######################################################################
#                         SETUP SOURCES                              #
######################################################################


# This is the directory containing the firmware package
STM_DIR=/Users/reecestevens/projects/freepulse/tools/STM32F4-Discovery_FW_V1.1.0

# This is where the source files are located,
# which are not in the current directory
SRC_DIRS = $(STM_DIR)/Libraries/STM32F4xx_StdPeriph_Driver/src
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
SRCS  += Adafruit_RA8875.cpp
SRCS  += Adafruit_GFX.cpp


# Startup file written by ST
SRCS += $(STM_DIR)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s

# The header files we use are located here
INC_DIRS  = $(STM_DIR)/Utilities/STM32F4-Discovery
INC_DIRS += $(STM_DIR)/Libraries/CMSIS/Include
INC_DIRS += $(STM_DIR)/Libraries/CMSIS/ST/STM32F4xx/Include
INC_DIRS += $(STM_DIR)/Libraries/STM32F4xx_StdPeriph_Driver/inc
INC_DIRS += ./libraries/inc
INC_DIRS += ./system/inc
INC_DIRS += ./inc

# OBJS = $(SRCS:.c=.o)

######################################################################
#                         SETUP TOOLS                                #
######################################################################

# This is the path to the toolchain
TOOLS_DIR = /Users/reecestevens/projects/freepulse/tools/gcc-arm-none-eabi-4_8-2014q1/bin
CC      = $(TOOLS_DIR)/arm-none-eabi-g++
OBJCOPY = $(TOOLS_DIR)/arm-none-eabi-objcopy
GDB     = $(TOOLS_DIR)/arm-none-eabi-gdb

INCLUDE = $(addprefix -I,$(INC_DIRS))

# #defines needed when working with the STM library
DEFS    = -DUSE_STDPERIPH_DRIVER

# if you use the following option, you must implement the function 
#    assert_failed(uint8_t* file, uint32_t line)
# because it is conditionally used in the library
# DEFS   += -DUSE_FULL_ASSERT

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

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(INCLUDE) $(DEFS) $(CFLAGS) $(LFLAGS) $^ -o $@ 
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf   $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash the STM32F4
flash: 
	st-flash write $(PROJ_NAME).bin 0x8000000

.PHONY: debug
debug:
# before you start gdb, you must start st-util
	$(GDB) $(PROJ_NAME).elf
