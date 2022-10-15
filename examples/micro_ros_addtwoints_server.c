#include <rtthread.h>
#include <rtdevice.h>
#include <micro_ros_rtt.h>

#if defined  MICROS_EXAMPLE_ADDTWOINTS_SERVER
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include "example_interfaces/srv/add_two_ints.h"
#include <std_msgs/msg/int64.h>

#include <stdio.h>
#include <unistd.h>

static rcl_service_t service;
static rcl_allocator_t allocator;
static rclc_support_t support;
static rcl_node_t node;
static rclc_executor_t executor;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rt_kprintf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rt_kprintf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}


static example_interfaces__srv__AddTwoInts_Request req;
static example_interfaces__srv__AddTwoInts_Response res;

void service_callback(const void * req, void * res){
   example_interfaces__srv__AddTwoInts_Request * req_in = (example_interfaces__srv__AddTwoInts_Request *) req;
   example_interfaces__srv__AddTwoInts_Response * res_in = (example_interfaces__srv__AddTwoInts_Response *) res;

  res_in->sum = req_in->a + req_in->b;
  rt_kprintf("Service request value: %d + %d.\n", (int)req_in->a, (int)req_in->b);
}

static void microros_addtwoint_server_thread_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(100);
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }
}


static void microros_addtwoints_server(int argc, char* argv[])
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
    RCCHECK(rclc_node_init_default(&node, "add_twoints_client_rclc", "", &support));
    // create service    
    RCCHECK(rclc_service_init_default(&service, &node, ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts), "/addtwoints"));

    // create executor
    RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
    RCCHECK(rclc_executor_add_service(&executor, &service, &req, &res, service_callback));

    rt_kprintf("[micro_ros] micro_ros init successful.\n");

    rt_thread_t thread = rt_thread_create("server_server", microros_addtwoint_server_thread_entry, RT_NULL, 2048, 25, 10);
    if(thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("[micro_ros] New thread server_server\n");
    }
    else
    {
        rt_kprintf("[micro_ros] Failed to create thread server_server\n");
    }

}

MSH_CMD_EXPORT(microros_addtwoints_server, micro ros addtwoints server example);

#endif