/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-11     hw630       the first version
 */

#include <micro_ros_rtt.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <std_msgs/msg/int32.h>

rclc_support_t support;
rcl_node_t node;

rcl_allocator_t allocator;
rcl_publisher_t publisher;

std_msgs__msg__Int32 msg;

// Functions create_entities and destroy_entities can take several seconds.
// In order to reduce this rebuild the library with
// - RMW_UXRCE_ENTITY_CREATION_DESTROY_TIMEOUT=0
// - UCLIENT_MAX_SESSION_CONNECTION_ATTEMPTS=3

void create_entities()
{
    allocator = rcl_get_default_allocator();

    // create init_options
    if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK)
    {
      rt_kprintf("[micro_ros] failed to initialize\n");
      return;
    };

    // create node
    if (rclc_node_init_default(&node, "microros_pub_int32_rclc", "", &support) != RCL_RET_OK)
    {
      rt_kprintf("[micro_ros] failed to create node\n");
      return;
    }
    rt_kprintf("[micro_ros] node created\n");

    // create publisher
    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "std_msgs_msg_Int32");

    rt_kprintf("[micro_ros] publisher created\n");
}

void destroy_entities()
{
    rcl_publisher_fini(&publisher, &node);
    rcl_node_fini(&node);
    rclc_support_fini(&support);
}

static void microros_deinitialization(int argc, char* argv[])
{
#if defined MICRO_ROS_USE_SERIAL
    // Serial setup
     set_microros_transports();
#endif

#if defined MICRO_ROS_USE_UDP
    // UDP setup
     if(argc==2) {
         set_microros_udp_transports(argv[1], 9999);
     }
     else {
         set_microros_udp_transports("192.168.1.100", 9999);
     }
#endif

    msg.data = 0;

    while (RMW_RET_OK != rmw_uros_ping_agent(50, 2));

    // Create publisher and node
    rt_kprintf("[micro_ros] creating entities\n");
    create_entities();

    // Publish messages
    for (int i = 0; i < 10; ++i)
    {
        rcl_publish(&publisher, &msg, NULL);
        rt_kprintf("[micro_ros] published %d\n", msg.data);

        msg.data++;
        rt_thread_mdelay(1000);

    }

    // Destroy publisher and node
    rt_kprintf("[micro_ros] destroying entities\n");
    destroy_entities();
    rt_kprintf("[micro_ros] destroyed entities\n");
}
MSH_CMD_EXPORT(microros_deinitialization, micro ros deinitialization example)
