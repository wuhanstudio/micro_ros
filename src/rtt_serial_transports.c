#include <rtthread.h>

#if defined MICRO_ROS_USE_SERIAL

#include "micro_ros_rtt.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#include <ctype.h>
#define _CTYPE_DATA_0_127 \
    _C, _C, _C, _C, _C, _C, _C, _C, \
    _C, _C|_S, _C|_S, _C|_S,    _C|_S,  _C|_S,  _C, _C, \
    _C, _C, _C, _C, _C, _C, _C, _C, \
    _C, _C, _C, _C, _C, _C, _C, _C, \
    _S|_B,  _P, _P, _P, _P, _P, _P, _P, \
    _P, _P, _P, _P, _P, _P, _P, _P, \
    _N, _N, _N, _N, _N, _N, _N, _N, \
    _N, _N, _P, _P, _P, _P, _P, _P, \
    _P, _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U, \
    _U, _U, _U, _U, _U, _U, _U, _U, \
    _U, _U, _U, _U, _U, _U, _U, _U, \
    _U, _U, _U, _P, _P, _P, _P, _P, \
    _P, _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L, \
    _L, _L, _L, _L, _L, _L, _L, _L, \
    _L, _L, _L, _L, _L, _L, _L, _L, \
    _L, _L, _L, _P, _P, _P, _P, _C
#define _CTYPE_DATA_128_255 \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0, \
    0,  0,  0,  0,  0,  0,  0,  0

char _ctype_b[128 + 256] = {
    _CTYPE_DATA_128_255,
    _CTYPE_DATA_0_127,
    _CTYPE_DATA_128_255
};

char __EXPORT *__ctype_ptr__ = (char *) _ctype_b + 127;


#define DBG_SECTION_NAME  "micro_ros_serial"
#define DBG_LEVEL         DBG_LOG
#include <rtdbg.h>

static struct rt_semaphore rx_sem;
static rt_device_t micro_ros_serial;

#ifndef MICRO_ROS_SERIAL_NAME
    #define MICRO_ROS_SERIAL_NAME "uart3"
#endif

#ifndef MICRO_ROS_SERIAL_TIMEOUT
    #define MICRO_ROS_SERIAL_TIMEOUT 500
#endif


// int clock_gettime(clockid_t unused, struct timespec *tp) __attribute__ ((weak));
// bool rtt_transport_open(struct uxrCustomTransport * transport) __attribute__ ((weak));
// bool rtt_transport_close(struct uxrCustomTransport * transport) __attribute__ ((weak));
// size_t rtt_transport_write(struct uxrCustomTransport * transport, uint8_t *buf, size_t len, uint8_t *errcode) __attribute__ ((weak));
// size_t rtt_transport_read(struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode) __attribute__ ((weak));

//#define micro_rollover_useconds 4294967295

int clock_gettime(clockid_t unused, struct timespec *tp)
{
    (void)unused;
//    static uint32_t rollover = 0;
//    static uint64_t last_measure = 0;

    uint64_t m = rt_tick_get() * 1000 / RT_TICK_PER_SECOND * 1000;
    tp->tv_sec = m / 1000000;
    tp->tv_nsec = (m % 1000000) * 1000;

    // Rollover handling
//    rollover += (m < last_measure) ? 1 : 0;
//    uint64_t rollover_extra_us = rollover * micro_rollover_useconds;
//    tp->tv_sec += rollover_extra_us / 1000000;
//    tp->tv_nsec += (rollover_extra_us % 1000000) * 1000;
//    last_measure = m;

    return 0;
}

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

bool rtt_transport_open(struct uxrCustomTransport * transport)
{
    micro_ros_serial = rt_device_find(MICRO_ROS_SERIAL_NAME);
    if (!micro_ros_serial)
    {
        LOG_E("Failed to open device %s", MICRO_ROS_SERIAL_NAME);
        return 0;
    }
    rt_sem_init(&rx_sem, "micro_ros_rx_sem", 0, RT_IPC_FLAG_FIFO);
    rt_device_open(micro_ros_serial, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(micro_ros_serial, uart_input);
    return 1;
}

bool rtt_transport_close(struct uxrCustomTransport * transport)
{
    rt_device_close(micro_ros_serial);
    rt_sem_detach(&rx_sem);
    return 1;
}

size_t rtt_transport_write(struct uxrCustomTransport * transport, const uint8_t *buf, size_t len, uint8_t *errcode)
{
    return rt_device_write(micro_ros_serial, 0, buf, len);
}

size_t rtt_transport_read(struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode)
{
//    int tick = rt_tick_get();
    for (int i = 0; i < len; ++i)
    {
        rt_sem_take(&rx_sem, timeout);
        rt_device_read(micro_ros_serial, -1, &buf[i], 1);
    }
//    while (rt_device_read(micro_ros_serial, -1, buf, len) != len)
//    {
//        rt_sem_take(&rx_sem, timeout);
//        if( (rt_tick_get() - tick) > timeout)
//        {
//            LOG_E("Read timeout");
//            return 0;
//        }
//    }
    return len;
}

#endif // MICRO_ROS_USE_SERIAL
