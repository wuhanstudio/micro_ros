#include <rtthread.h>
#include <micro_ros_rtt.h>
//#include <IMU.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <geometry_msgs/msg/transform_stamped.h>
#include <tf2_msgs/msg/tf_message.h>

#include <micro_ros_utilities/type_utilities.h>
#include <micro_ros_utilities/string_utilities.h>

rcl_publisher_t publisher;
tf2_msgs__msg__TFMessage * tf_message;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){return;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


static void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  RCLC_UNUSED(timer);
}

static void micro_ros_example(int argc, char* argv[])
{
  set_microros_transports();

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(tf2_msgs, msg, TFMessage),
    "/tf"));

  // create timer,
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

  if(!micro_ros_utilities_create_message_memory(
      ROSIDL_GET_MSG_TYPE_SUPPORT(tf2_msgs, msg, TFMessage),
      &tf_message,
      (micro_ros_utilities_memory_conf_t) {})
    )
  {
    return;
  }

  tf_message->transforms.size = 2;

  tf_message->transforms.data[0].header.frame_id =
    micro_ros_string_utilities_set(tf_message->transforms.data[0].header.frame_id, "/panda_link0");

  tf_message->transforms.data[1].header.frame_id =
    micro_ros_string_utilities_set(tf_message->transforms.data[1].header.frame_id, "/inertial_unit");

    while(1)
	{
	  struct timespec tv = {0};
	  clock_gettime(0, &tv);

//	  IMU.update();
	  double q[4];
//	  euler_to_quat(IMU.rpy[0], IMU.rpy[1], IMU.rpy[2], q);

	  tf_message->transforms.data[0].transform.rotation.x = (double) q[1];
		tf_message->transforms.data[0].transform.rotation.y = (double) q[2];
		tf_message->transforms.data[0].transform.rotation.z = (double) q[3];
		tf_message->transforms.data[0].transform.rotation.w = (double) q[0];
	  tf_message->transforms.data[0].header.stamp.nanosec = tv.tv_nsec;
		tf_message->transforms.data[0].header.stamp.sec = tv.tv_sec;

	  RCSOFTCHECK(rcl_publish(&publisher, tf_message, NULL));

	  //RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
	}
}
MSH_CMD_EXPORT(micro_ros_example, micro ros tf publisher)
