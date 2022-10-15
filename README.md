

# README

## 概述

[2022开源之夏](https://summer-ospp.ac.cn/#/org/prodetail/22f330436)项目：microROS 添加 RT-Thread 支持。

该分支可以实现在rtthread工程下构建microROS（galactic）静态库

## 依赖

* colcon
* cmake

## 测试硬件

* [STMH750-ART-Pi](https://github.com/RT-Thread-Studio/sdk-bsp-stm32h750-realthread-artpi)

## 使用

### 1. 下载code

rtthread  code

```bash
# 下载rtthread  （对于其他单片机，使用rtthread仓库）
git clone https://github.com/RT-Thread-Studio/sdk-bsp-stm32h750-realthread-artpi.git
# 进入目标bsp
cd cd sdk-bsp-stm32h750-realthread-artpi/projects/art_pi_wifi/
git clone https://github.com/navy-to-haijun/micro-ROS-rtthread-app.git
```

micro-ROS-rtthread-app:

```bash
.
├── examples									# demo
│   ├── micro_ros_addtwoints_client.c
│   ├── micro_ros_addtwoints_server.c
│   ├── micro_ros_ping_pong.c
│   ├── micro_ros_pub_int32.c
│   ├── micro_ros_pub_sub_int32.c
│   └── micro_ros_sub_int32.c
├── Kconfig										# 可以通过 scons --menuconfig 配置 microROS的通信方式和使用的demo
├── microros									# 建立microROS的脚本文件
│   ├── colcon.meta								# microROS相关包的宏定义，可以根据单片机的内存大小，修改支持的话题、服务、节点的数目
│   ├── generate_microros_library.sh			# 下载，编译microROS
│   └── toolchain.cmake.in						
├── README.md
├── rtconfig.h									# 脚本模式下，无用
├── SConscript
└── transports									# 通信方式
    ├── micro_ros_rtt.h
    ├── rtt_serial_transports.c
    └── rtt_udp_transport.c
```

### 3. 配置UDP/UART 

自我配置，确保基本可以通信成功

### 3. rtthead 支持 cmake构建

```bash
# 在rtconfig.py 中 添加 gcc-arm-none-eabi-5_4 的编译路径

if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
    EXEC_PATH   = r'/home/haijun/env_released_1.2.0/gcc-arm-none-eabi-5_4-2016q3/bin'

# art- pi 需要
ln -s ../../libraries/ libraries
ln -s ../../rt-thread/ rt-thread
# 支持cmake构建
scons --target=cmake
```

### 4. 修改 工程目录下的`CMakeLists.txt`，支持microROS， 添加通信方式、添加demo

代码添加到`ADD_EXECUTABLE()`之前

```cmake

# build micro-ROS : make build_microros
add_custom_target(build_microros
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/micro-ROS-rtthread-app/microros"
	COMMAND sh generate_microros_library.sh ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER} ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}
	COMMENT "build micro-ROS..."
)

# head files
INCLUDE_DIRECTORIES(micro-ROS-rtthread-app/microros/build/include)
INCLUDE_DIRECTORIES(micro-ROS-rtthread-app/transports)

#  teansport: serial
add_definitions(-DMICROROS_SERIAL)
add_definitions(-DMICROROS_DEVIVE="vcom")
list(APPEND PROJECT_SOURCES micro-ROS-rtthread-app/transports/rtt_serial_transports.c)

# example 
# pub_int32
add_definitions(-DMICROS_EXAMPLE_PUB_INT32)
list(APPEND PROJECT_SOURCES micro-ROS-rtthread-app/examples/micro_ros_pub_int32.c)

link_directories(${CMAKE_SOURCE_DIR}/micro-ROS-rtthread-app/microros/build)
LINK_LIBRARIES(microros)

```

```bash
cd build
cmke ..
make build_microros
make
```

然后可以下载可执行文件

macroROS 静态库和头文件路径:`micro-ROS-rtthread-app/microros/build`

编译可能会报错：

```bash
In file included from micro-ROS-rtthread-app/microros/build/include/rmw/qos_profiles.h:23:0,
                 from micro-ROS-rtthread-app/microros/build/include/rmw/rmw.h:103,
                 from micro-ROS-rtthread-app/microros/build/include/rmw_microros/rmw_microros.h:20,
                 from micro-ROS-rtthread-app/transports/micro_ros_rtt.h:9,
                 from micro-ROS-rtthread-app/examples/micro_ros_pub_int32.c:3:
micro-ROS-rtthread-app/microros/build/include/rmw/types.h:418:49: error: expected ',' or '}' before '__attribute__'
 # define RMW_DECLARE_DEPRECATED(name, msg) name __attribute__((deprecated(msg))
                                                 ^
micro-ROS-rtthread-app/microros/build/include/rmw/types.h:438:3: note: in expansion of macro 'RMW_DECLARE_DEPRECATED'
   RMW_DECLARE_DEPRECATED(
   ^
In file included from micro-ROS-rtthread-app/microros/build/include/rmw/rmw.h:103:0,
                 from micro-ROS-rtthread-app/microros/build/include/rmw_microros/rmw_microros.h:20,
                 from micro-ROS-rtthread-app/transports/micro_ros_rtt.h:9,
                 from micro-ROS-rtthread-app/examples/micro_ros_pub_int32.c:3:
micro-ROS-rtthread-app/microros/build/include/rmw/qos_profiles.h:111:3: error: 'RMW_QOS_POLICY_LIVELINESS_UNKNOWN' undeclared here (not in a function)
   RMW_QOS_POLICY_LIVELINESS_UNKNOWN,
```

修改：`micro-ROS-rtthread-app/microros/build/include/rmw/types.h` 418行

```c
#ifndef _WIN32
//# define RMW_DECLARE_DEPRECATED(name, msg) name __attribute__((deprecated(msg)))
# define RMW_DECLARE_DEPRECATED(name, msg) name
```

### 5. 下载 运行

```shell
docker run -it --rm -v /dev:/dev --privileged --net=host microros/micro-ros-agent:galactic serial -D /dev/ttyACM1 -v6
# or
docker run -it --rm -v /dev:/dev --privileged --net=host microros/micro-ros-agent:galactic udp4 --port 9999 -v6
```

### 额外说明

如果不喜欢cmake编译，可以只使用cmake编译microROS，然后使用scons编译

```
# 1. 在CMakeLists.txt添加microROS编译成静态库的code

# build micro-ROS : make build_microros
add_custom_target(build_microros
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/micro-ROS-rtthread-app/microros"
	COMMAND sh generate_microros_library.sh ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER} ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}
	COMMENT "build micro-ROS..."
)

# 2. 在Kcongfig中source micro-ROS-rtthread-app 的Kconfig
source micro-ROS-rtthread-app/Kconfig
# 3. 配置通信方式和demo
 sons --menuconfig
# 4. 编译
sons
```

## 总结

推荐使用shell脚本编译microROS，自由度较大，若出现编译错误，请检查是否路径有问题。
