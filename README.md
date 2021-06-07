# MicroROS on RT-Thread
<img src="./docs/micro-ROS_big_logo.png" >

> 默认使用 Cortex-M3 测试，已经实现和 ROS2 Foxy 的串口连接

#### Step 1: Starting micro-ros-agent using docker

```
$ docker run -it -v /dev:/dev --privileged microros/micro-ros-agent:foxy serial --dev /dev/ttyUSB0
```
#### Step 2: Starting microros publisher on RT-Thread
```
msh >microros_pub_int32
[micro_ros] node created
[micro_ros] publisher created
[micro_ros] timer created
[micro_ros] executor created
[micro_ros] New thread mr_pubint32
```
#### Step 3: Echoing messages

Once connected, you should see the output:

```
Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0
[1623057529.937043] info     | TermiosAgentLinux.cpp | init                     | running...             | fd: 4
[1623057529.937150] info     | Root.cpp           | set_verbose_level        | logger setup           | verbose_level: 4
[1623057541.764331] info     | Root.cpp           | create_client            | create                 | client_key: 0x6F7B427A, session_id: 0x81
[1623057541.764507] info     | SessionManager.hpp | establish_session        | session established    | client_key: 0x1870348922, address: 0
```
And the corresponding topic:

```
$ docker exec -it YOUR_DOCKER_CONTAINER_NAME /bin/bash
$ ros2 topic echo /micro_ros_rtt_node_publisher
data: 0
---
data: 1
---
data: 2
---
data: 3
```

#### Todo List

- 更多 Example
- 实现 UDP 连接
- 测试更多 MCU:  Cortex-M0, Cortex-M4, Cortex-M7
