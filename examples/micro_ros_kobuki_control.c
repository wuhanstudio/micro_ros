/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-12     hw630       the first version
 */

#include <micro_ros_rtt.h>
#include <kobuki.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <geometry_msgs/msg/twist.h>

rcl_subscription_t subscriber;
geometry_msgs__msg__Twist msg;
rclc_executor_t executor;
rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;

static struct kobuki robot;
static double linear_x = 0;
static double angular_z = 0;

static void kobuki_entry(void *parameter)
{
    kobuki_init(&robot);
    while(1)
    {
        rt_thread_mdelay(100);
        kobuki_set_speed(linear_x, angular_z);
    }
    kobuki_close(&robot);
}

static void microros_kobuki_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(100);
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }
}

//twist message cb
static void kobuki_callback(const void *msgin) {
    // Please remember to enable -u_printf_float in compiler settings
    const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;
    printf("linear \n");
    printf("  x: %f \n", msg->linear.x);
    printf("angular \n");
    printf("  z: %f \n", msg->angular.z);
    linear_x = msg->linear.x;
    angular_z = msg->angular.z;
}

void microros_kobuki_control(int argc, char* argv[])
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

    allocator = rcl_get_default_allocator();

    // create init_options
    if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK)
    {
        rt_kprintf("[micro_ros] failed to initialize\n");
        return;
    };

    // create node
    if (rclc_node_init_default(&node, "micro_ros_rtt_sub_twist", "", &support) != RCL_RET_OK)
    {
        rt_kprintf("[micro_ros] failed to create node\n");
        return;
    }

    // create subscriber
    rclc_subscription_init_default(
      &subscriber,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
      "cmd_vel");

    // create executor
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_subscription(&executor, &subscriber, &msg, &kobuki_callback, ON_NEW_DATA);

    rt_thread_t thread = rt_thread_create("mr_kobuki", microros_kobuki_entry, RT_NULL, 2048, 15, 10);
    if(thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("[micro_ros] New thread mr_kobuki\n");
    }
    else
    {
        rt_kprintf("[micro_ros] Failed to create thread mr_kobuki\n");
    }

    rt_thread_t threadk = rt_thread_create("kobuki", kobuki_entry, RT_NULL, 2048, 10, 10);
    if(threadk != RT_NULL)
    {
        rt_thread_startup(threadk);
        rt_kprintf("[micro_ros] New thread kobuki\n");
    }
    else
    {
        rt_kprintf("[micro_ros] Failed to create thread kobuki\n");
    }
}
MSH_CMD_EXPORT(microros_kobuki_control, micro ros control kobuki robot example)
