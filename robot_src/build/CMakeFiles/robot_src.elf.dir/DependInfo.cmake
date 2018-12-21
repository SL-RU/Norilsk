# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  "C"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/lyra/tmp/Norilsk/robot_src/startup/startup_stm32f103xb.s" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/startup/startup_stm32f103xb.s.o"
  )
set(CMAKE_ASM_COMPILER_ID "Clang")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "IS_STM32=1"
  "STM32F103xB"
  "USE_HAL_DRIVER"
  "__packed=__attribute__((__packed__))"
  "__weak=__attribute__((weak))"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "../Core/Inc"
  "../Drivers/CMSIS/Device/ST/STM32F1xx/Include"
  "../Drivers/CMSIS/Include"
  "../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy"
  "../Drivers/STM32F1xx_HAL_Driver/Inc"
  "../nrf24"
  )
set(CMAKE_DEPENDS_CHECK_C
  "/home/lyra/tmp/Norilsk/robot_src/Core/Src/gpio.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Core/Src/gpio.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Core/Src/main.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Core/Src/main.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Core/Src/spi.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Core/Src/spi.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Core/Src/stm32f1xx_hal_msp.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Core/Src/stm32f1xx_hal_msp.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Core/Src/stm32f1xx_it.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Core/Src/stm32f1xx_it.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Core/Src/system_stm32f1xx.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Core/Src/system_stm32f1xx.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Core/Src/tim.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Core/Src/tim.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Core/Src/usart.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Core/Src/usart.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi_ex.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi_ex.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c.o"
  "/home/lyra/tmp/Norilsk/robot_src/nrf24/nrf24.c" "/home/lyra/tmp/Norilsk/robot_src/build/CMakeFiles/robot_src.elf.dir/nrf24/nrf24.c.o"
  )
set(CMAKE_C_COMPILER_ID "Clang")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_C
  "IS_STM32=1"
  "STM32F103xB"
  "USE_HAL_DRIVER"
  "__packed=__attribute__((__packed__))"
  "__weak=__attribute__((weak))"
  )

# The include file search paths:
set(CMAKE_C_TARGET_INCLUDE_PATH
  "../Core/Inc"
  "../Drivers/CMSIS/Device/ST/STM32F1xx/Include"
  "../Drivers/CMSIS/Include"
  "../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy"
  "../Drivers/STM32F1xx_HAL_Driver/Inc"
  "../nrf24"
  )

# Targets to which this target links.
set(CMAKE_TARGET_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
