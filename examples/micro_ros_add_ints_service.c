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
#include <micro_ros_rtt.h>
#include <example_interfaces/srv/add_two_ints.h>

#include <stdio.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int64.h>

rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;

rcl_service_t service;
rcl_wait_set_t wait_set;

example_interfaces__srv__AddTwoInts_Response res;
example_interfaces__srv__AddTwoInts_Request req;

static void microros_add_ints_thread_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(100);
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }
}

void service_callback(const void * req, void * res)
{
    // Now you can call the service
    // sudo apt-get install ros-foxy-example-interfaces
    // ros2 service call /addtwoints example_interfaces/srv/AddTwoInts "{a: 1, b: 2}"

    // You should see the result
    // example_interfaces.srv.AddTwoInts_Response(sum=3)

    // Warn: make sure there is only one node for the service
    // ros2 node list
    example_interfaces__srv__AddTwoInts_Request * req_in = (example_interfaces__srv__AddTwoInts_Request *) req;
    example_interfaces__srv__AddTwoInts_Response * res_in = (example_interfaces__srv__AddTwoInts_Response *) res;

    printf("Service request value: %d + %d.\n", (int) req_in->a, (int) req_in->b);

    res_in->sum = req_in->a + req_in->b;
}

static void microros_add_ints_service(int argc, char* argv[])
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
    if (rclc_node_init_default(&node, "micro_ros_rtt_add_ints_rclc", "", &support) != RCL_RET_OK)
    {
        rt_kprintf("[micro_ros] failed to create node\n");
        return;
    }

    // create service
    if ( rclc_service_init_default(&service, &node, ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts), "/addtwoints") != RCL_RET_OK)
    {
        rt_kprintf("[micro_ros] failed to create service\n");
        return;
    }

    // create executor
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_service(&executor, &service, &req, &res, service_callback);

    rt_thread_t thread = rt_thread_create("mr_add_svc", microros_add_ints_thread_entry, RT_NULL, 2048, 25, 10);
    if(thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("[micro_ros] New thread mr_add_svc\n");
    }
    else
    {
        rt_kprintf("[micro_ros] Failed to create thread mr_add_svc\n");
    }
}
MSH_CMD_EXPORT(microros_add_ints_service, micro ros add ints service example)
