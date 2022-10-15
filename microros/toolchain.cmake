set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

SET(CMAKE_C_COMPILER_WORKS 1 CACHE INTERNAL "")
SET(CMAKE_CXX_COMPILER_WORKS 1 CACHE INTERNAL "")

set(CMAKE_C_COMPILER /home/haijun/env_released_1.2.0/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /home/haijun/env_released_1.2.0/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-g++)

set(CMAKE_C_FLAGS_INIT " -fno-exceptions -nostdlib -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections  -O0   -std=c99 -DCLOCK_MONOTONIC=0 -D'__attribute__(x)='  " CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_INIT " -std=c++14  -fno-rtti -nostdlib -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections  -O0   -DCLOCK_MONOTONIC=0 -D'__attribute__(x)=' " CACHE STRING "" FORCE)

set(__BIG_ENDIAN__ 0)