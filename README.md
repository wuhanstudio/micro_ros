

# README

## 概述

[2022开源之夏](https://summer-ospp.ac.cn/#/org/prodetail/22f330436)项目：microROS 添加 RT-Thread 支持。

该分支可以实现在rtthread工程下构建microROS（galactic）静态库

## 依赖

* colcon
* cmake
* gcc-10

## 限制条件

* 在linux下使用
* 请提前配置和确认串口或者UDP能正常收发数据；

## 测试硬件

* [STMH750-ART-Pi](https://github.com/RT-Thread-Studio/sdk-bsp-stm32h750-realthread-artpi)

## 使用

### 1. 下载code

rtthread  code

```bash
# 下载rtthread  （对于其他单片机，使用rtthread仓库）
git clone https://github.com/RT-Thread-Studio/sdk-bsp-stm32h750-realthread-artpi.git
# 进入目标bsp
cd sdk-bsp-stm32h750-realthread-artpi/projects/art_pi_wifi/
# art- pi 需要
ln -s ../../libraries/ libraries
ln -s ../../rt-thread/ rt-thread
# 下载 microROS 编译脚本 demo
cd packages
git clone -b galactic-standalone-gcc-10 https://github.com/wuhanstudio/micro_ros.git
```

关于 microros 文件夹说明

```bash
.
├── examples							# demo 文件夹
│   ├── micro_ros_addtwoints_server.c	
│   ├── micro_ros_ping_pong.c			
│   ├── micro_ros_pub_int32.c			
│   ├── micro_ros_pub_sub_int32.c		
│   ├── micro_ros_sub_int32.c			
│   └── micro_ros_sub_twist.c
├── Kconfig							# 实现 menuconfig 配置通信方式和使用demo
├── microros
│   ├── colcon.meta					# microROS配置文件
│   ├── generate_microros_library.sh# 构建microROS静态库核心脚本
│   └── toolchain.cmake.in			# 编译microROS预定义宏文件
├── microros_library.sh				# 编译运行脚本
├── README.md
├── SConscript						# 将microROS相关的code配置到rtthread中
└── transports						# UDP serial 接口
    ├── micro_ros_rtt.h
    ├── rtt_serial_transports.c
    └── rtt_udp_transport.c

```

其中`microros_library.sh`脚本实现以下功能：

* 使rtthread支持cmake编译;
* 在`CMakeLists.txt`中加入自定义命令，使其能编译microROS
* 调用`microros/generate_microros_library.sh`
* 在`Kconfig`中source `microros`中的`Kconfig`

### 2. 配置交叉编译链路径

在`rtconfig.py`中配置交叉编译链路径：example

```python
if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
    EXEC_PATH   = r'/home/haijun/env_released_1.2.0/gcc-arm-none-eabi-10.3-2021.10/bin'
```

### 3. 编译microROS静态库

```bash
cd micro_ros/
./microros_library.sh
```

### 4. 自我配置UART和UDP驱动

UART：确保`rt_device_write()`和 `rt_device_read()`正常；

UDP：确保`send()` 和 `recv()`正常；

### 5. 配置通信方式和使用脚本

`scons --menuconfig`

art-pi 使用虚拟串口

编译：`scons`

### 6. 运行

```bash
docker run -it --rm -v /dev:/dev --privileged --net=host microros/micro-ros-agent:galactic serial -D /dev/ttyACM1 -v6
# or
docker run -it --rm -v /dev:/dev --privileged --net=host microros/micro-ros-agent:galactic udp4 --port 9999 -v6
```
