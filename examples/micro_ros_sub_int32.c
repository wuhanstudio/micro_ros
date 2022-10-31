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
#include <rtdevice.h>
#include <micro_ros_rtt.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>

rcl_subscription_t subscriber;
std_msgs__msg__Int32 msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

#define LED_PIN 45  // PC13

static void subscription_callback(const void * msgin)
{
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
    rt_kprintf("[micro_ros] received data %d\n", msg->data);
    rt_pin_write(LED_PIN, (msg->data == 0) ? 0 : 1);
}

static void microros_sub_int32_thread_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(100);
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }
}

static void microros_sub_int32(int argc, char* argv[])
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

    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);

    allocator = rcl_get_default_allocator();

    //create init_options
    if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK)
    {
        rt_kprintf("[micro_ros] failed to initialize\n");
        return;
    };

    // create node
    if (rclc_node_init_default(&node, "micro_ros_rtt_sub_int32_node", "", &support) != RCL_RET_OK)
    {
        rt_kprintf("[micro_ros] failed to create node\n");
        return;
    }
    rt_kprintf("[micro_ros] node created\n");

    // create subscriber
    rclc_subscription_init_default(
      &subscriber,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "micro_ros_rtt_subscriber");

    // create executor
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA);
    rt_kprintf("[micro_ros] executor created\n");

    rt_thread_t thread = rt_thread_create("mr_subint32", microros_sub_int32_thread_entry, RT_NULL, 2048, 25, 10);
    if(thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("[micro_ros] New thread mr_subint32\n");
    }
    else
    {
        rt_kprintf("[micro_ros] Failed to create thread mr_subint32\n");
    }

    // now you can publish a message to turn on/off the LED
    // ros2 topic pub --once /micro_ros_rtt_subscriber std_msgs/msg/Int32 data:\ 0
}
MSH_CMD_EXPORT(microros_sub_int32, micro ros subscribe int32 example)
