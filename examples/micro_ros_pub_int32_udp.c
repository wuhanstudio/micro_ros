/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-11     hw630       the first version
 */

#include <rtthread.h>

#if defined MICRO_ROS_USE_UDP

#include <micro_ros_rtt.h>
#include <rtthread.h>
#include <stdio.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>

static rcl_publisher_t publisher;
static std_msgs__msg__Int32 msg;

static rclc_executor_t executor;
static rclc_support_t support;
static rcl_allocator_t allocator;

static rcl_node_t node;

#define LOOP_RATE 1000

static void microros_pub_int32_thread_entry(void *parameter)
{
    while(1)
    {
        rt_tick_t start_loop_time = rt_tick_get();

        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
        rcl_publish(&publisher, &msg, NULL);
        msg.data++;

        // If loop rate is less than required, wait necessary time
        while (rt_tick_get() - start_loop_time < (1000000.0 / LOOP_RATE))
        {
            rt_thread_mdelay(10);
        }
    }
}


static void microros_pub_int32_udp(int argc, char* argv[])
{
    set_microros_udp_transports("192.168.199.100", 9999);

    allocator = rcl_get_default_allocator();

    //create init_options
    if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK)
    {
        rt_kprintf("[micro_ros] failed to initialize\n");
        return;
    };

    // create node
    if (rclc_node_init_default(&node, "micro_ros_rtt_udp_node", "", &support) != RCL_RET_OK)
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
        "micro_ros_rtt_udp_node_publisher");

    rt_kprintf("[micro_ros] publisher created\n");

    // create executor
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rt_kprintf("[micro_ros] executor created\n");

    msg.data = 0;

    rt_thread_t thread = rt_thread_create("mr_pubint32_udp", microros_pub_int32_thread_entry, RT_NULL, 2048, 25, 10);
    if(thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("[micro_ros] New thread mr_pubint32_udp\n");
    }
    else
    {
        rt_kprintf("[micro_ros] Failed to create thread mr_pubint32_udp\n");
    }
}
MSH_CMD_EXPORT(microros_pub_int32_udp, microros publish int32 udp example)

#endif // MICRO_ROS_USE_UDP
