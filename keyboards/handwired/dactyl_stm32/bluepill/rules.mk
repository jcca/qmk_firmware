# MCU name
MCU = STM32F103

# Bootloader selection
BOOTLOADER = stm32duino

# Enter lower-power sleep mode when on the ChibiOS idle thread
OPT_DEFS += -DCORTEX_ENABLE_WFI_IDLE=TRUE

CUSTOM_MATRIX = yes     # Custom matrix file
QUANTUM_LIB_SRC += i2c_master.c
SRC = matrix.c
