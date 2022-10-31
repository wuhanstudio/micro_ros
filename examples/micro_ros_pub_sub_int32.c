
#include <rtthread.h>
#include <rtdevice.h>
#include <micro_ros_rtt.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rt_kprintf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rt_kprintf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

static rcl_allocator_t allocator;
static rclc_support_t support;
static rcl_node_t node;
static rcl_timer_t timer;
static rclc_executor_t executor;

static rcl_publisher_t publisher;
static rcl_subscription_t subscriber;
static std_msgs__msg__Int32 send_msg;
static std_msgs__msg__Int32 recv_msg;


static void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	(void) last_call_time;
	if (timer != NULL) {
		RCSOFTCHECK(rcl_publish(&publisher, &send_msg, NULL));
		rt_kprintf("Sent: %d\n", send_msg.data);
		send_msg.data++;
	}
}

static void subscription_callback(const void * msgin)
{
	const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
	rt_kprintf("Received: %d\n", msg->data);
}

static void microros_pub_sub_thread_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(100);
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }
}

static void microros_pub_sub(int argc, char* argv[])
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
	RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
	// create node
	RCCHECK(rclc_node_init_default(&node, "int32_publisher_rclc", "", &support));

	// create publisher
	RCCHECK(rclc_publisher_init_default(
		&publisher,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
		"std_msgs_msg_Int32"));
	
    RCCHECK(rclc_subscription_init_default(
		&subscriber,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
		"int32_subscriber"));

	// create timer,
	const unsigned int timer_timeout = 1000;
	RCCHECK(rclc_timer_init_default(
		&timer,
		&support,
		RCL_MS_TO_NS(timer_timeout),
		timer_callback));

	// create executor
	executor = rclc_executor_get_zero_initialized_executor();
	RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
	RCCHECK(rclc_executor_add_timer(&executor, &timer));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &recv_msg, &subscription_callback, ON_NEW_DATA));

    rt_thread_t thread = rt_thread_create("pub_sub_int32", microros_pub_sub_thread_entry, RT_NULL, 2048, 18, 10);
    if(thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("[micro_ros] New thread pub_sub_int32\n");
    }
    else
    {
        rt_kprintf("[micro_ros] Failed to create thread pub_sub_int32\n");
    }

}
MSH_CMD_EXPORT(microros_pub_sub, microros pub_sub example)

// ros2 topic pub -r  2 /int32_subscriber std_msgs/msg/Int32 data:\ 0 
