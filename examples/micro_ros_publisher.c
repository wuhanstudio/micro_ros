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
static rcl_timer_t timer;

static void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{  
    // RCLC_UNUSED(last_call_time);
    rt_kprintf("callback \n");
    if (timer != NULL) 
    {
        rcl_publish(&publisher, &msg, NULL);
        msg.data++;
    }
    else {
        rt_kprintf("timer null\n");
    }
}

static void microros_publisher(int argc, char* argv[])
{
    set_microros_transports();

    rt_thread_mdelay(2000);

    allocator = rcl_get_default_allocator();

    //create init_options
    if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK)
    {
        rt_kprintf("Failed to initialize\n");
    };

    // create node
     rclc_node_init_default(&node, "micro_ros_rtt_node", "", &support);

    // create publisher
     rclc_publisher_init_default(
       &publisher,
       &node,
       ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
       "micro_ros_rtt_node_publisher");

    // create timer,
     const unsigned int timer_timeout = 1000;
     rclc_timer_init_default(
       &timer,
       &support,
       RCL_MS_TO_NS(timer_timeout),
       timer_callback);

     // create executor
     rclc_executor_init(&executor, &support.context, 1, &allocator);
     rclc_executor_add_timer(&executor, &timer);

     msg.data = 0;

     while(1)
     {
       rt_thread_mdelay(100);
       rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
     }

}
MSH_CMD_EXPORT(microros_publisher, microros publisher)
