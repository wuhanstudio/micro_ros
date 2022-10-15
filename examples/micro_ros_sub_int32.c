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
#if defined  MICROS_EXAMPLE_SUB_INT32 

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>

#include <stdio.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rt_kprintf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rt_kprintf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

static rcl_publisher_t publisher;
static rcl_subscription_t subscriber;
static std_msgs__msg__Int32 recv_msg;
static rclc_executor_t executor;
static rclc_support_t support;
static rcl_allocator_t allocator;
static rcl_node_t node;

static int count= 0;
static void subscription_callback(const void * msgin)
{
    count ++;
	const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
	rt_kprintf("%d: Received: %d\n", count, msg->data);
}

static void microros_sub_int32_thread_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(100);
        RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
    }
}

static void microros_sub_int32(int argc, char* argv[])
{
    #if defined MICROROS_USE_SERIAL
    // Serial setup
    	set_microros_transports();
	#endif

	#if defined MICROROS_USE_UDP
    // TCP setup
     	set_microros_udp_transports(MICROROS_IP, MICROROS_PORT);
	#endif

    allocator = rcl_get_default_allocator();
    // create init_options
	RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
    // create node
	RCCHECK(rclc_node_init_default(&node, "micro_ros_rtt_node", "", &support));

	// create subscriber
	RCCHECK(rclc_subscription_init_default(
		&subscriber,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
		"micro_ros_rtt_node_subscriber"));
    
    // create executor
	RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
	RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &recv_msg, &subscription_callback, ON_NEW_DATA));

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

    // ros2 topic pub /micro_ros_rtt_node_subscriber std_msgs/msg/Int32 data:\ 12
}
MSH_CMD_EXPORT(microros_sub_int32, micro ros subscribe int32 example)
#endif
