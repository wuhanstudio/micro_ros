
#ifndef MICRO_ROS_RTT
#define MICRO_ROS_RTT

// ---- Build fixes -----
//Removing __attribute__ not supported by gcc-arm-none-eabi-5_4
#define __attribute__(x)

#include <uxr/client/transport.h>
#include <rmw_microros/rmw_microros.h>

bool   rtt_transport_open(struct uxrCustomTransport * transport);
bool   rtt_transport_close(struct uxrCustomTransport * transport);
size_t rtt_transport_write(struct uxrCustomTransport* transport, uint8_t * buf, size_t len, uint8_t * err);
size_t rtt_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);
int clock_gettime(clockid_t unused, struct timespec *tp);

static inline void set_microros_transports(){
	rmw_uros_set_custom_transport(
		true,
		NULL,
		rtt_transport_open,
		rtt_transport_close,
		rtt_transport_write,
		rtt_transport_read
	);
}

#if defined(TARGET_PORTENTA_H7_M7) || defined(ARDUINO_NANO_RP2040_CONNECT)

#if defined(TARGET_PORTENTA_H7_M7)
#include <WiFi.h>
#include <WiFiUdp.h>
#else defined(ARDUINO_NANO_RP2040_CONNECT)
#include <SPI.h>
#include <WiFiNINA.h>
#endif

extern "C" bool arduino_wifi_transport_open(struct uxrCustomTransport * transport);
extern "C" bool arduino_wifi_transport_close(struct uxrCustomTransport * transport);
extern "C" size_t arduino_wifi_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
extern "C" size_t arduino_wifi_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

struct micro_ros_agent_locator {
	IPAddress address;
	int port;
};

static inline void set_microros_wifi_transports(char * ssid, char * pass, char * agent_ip, uint agent_port){
	while (WiFi.begin(ssid, pass) != WL_CONNECTED)
	{
    	delay(500);
	}

	static struct micro_ros_agent_locator locator;
	locator.address.fromString(agent_ip);
	locator.port = agent_port;

	rmw_uros_set_custom_transport(
		false,
		(void *) &locator,
		arduino_wifi_transport_open,
		arduino_wifi_transport_close,
		arduino_wifi_transport_write,
		arduino_wifi_transport_read
	);
}

#endif

#endif  // MICRO_ROS_RTT
