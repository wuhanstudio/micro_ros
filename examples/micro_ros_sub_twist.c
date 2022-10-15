#include <rtthread.h>
#include <rtdevice.h>
#include <micro_ros_rtt.h>
#if defined  MICROS_EXAMPLE_SUB_TWIST
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>

static rcl_subscription_t subscriber;
static geometry_msgs__msg__Twist msg;
static rclc_executor_t executor;
static rcl_allocator_t allocator;
static rclc_support_t support;
static rcl_node_t node;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rt_kprintf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rt_kprintf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

static void microros_sub_twist_thread_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(100);
        RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
    }
}

//twist message cb
void subscription_callback(const void *msgin) {
  // Please remember to enable -u_printf_float in compiler settings
  const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;
  printf("linear \n");
  printf("  x: %f \n", msg->linear.x);
  printf("  y: %f \n", msg->linear.y);
  printf("  z: %f \n", msg->linear.z);
  printf("angular \n");
  printf("  x: %f \n", msg->angular.x);
  printf("  y: %f \n", msg->angular.y);
  printf("  z: %f \n", msg->angular.z);
}

static void microros_sub_twist(int argc, char* argv[])
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
    RCCHECK(rclc_node_init_default(&node, "micro_ros_rtt_sub_twist", "", &support));
    // create subscriber
    RCCHECK(rclc_subscription_init_default(
		&subscriber,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
		"cmd_vel"));
    
    // // create executor
    RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
	RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));

    rt_thread_t thread = rt_thread_create("mr_sub_twist", microros_sub_twist_thread_entry, RT_NULL, 2048, 25, 10);
    if(thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("[micro_ros] New thread mr_sub_twist\n");
    }
    else
    {
        rt_kprintf("[micro_ros] Failed to create thread mr_sub_twist\n");
    }
}
MSH_CMD_EXPORT(microros_sub_twist, micro ros subscribe to twist example)
#endif