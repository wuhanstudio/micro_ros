# MicroROS on RT-Thread (galactic)
<img src="./docs/micro-ROS_big_logo.png" >

Communication between the Micro-ROS Agent and the Micro-ROS nodes supports two types of transport:

- UDP over IPv4 and IPv6.
- Serial Port transports.

> By default, gcc 5.4.1 is used to generate static libraries in extras/library_generation/library_generation.sh

#### Step 1: Starting micro-ros-agent using docker

For serial port:

```
$ docker run -it -v /dev:/dev --privileged microros/micro-ros-agent:galactic serial --dev /dev/ttyUSB0
```
For udp port:

```
$ docker run -it -p 9999:9999/udp --privileged microros/micro-ros-agent:galactic udp4 -p 9999
```

#### Step 2: Starting microros publisher on RT-Thread

```
 \ | /
- RT -     Thread Operating System
 / | \     4.0.4 build Jun  9 2021
 2006 - 2021 Copyright by rt-thread team
msh >
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
