//#include<rtconfig.h>


#include <rtthread.h>
#include "micro_ros_rtt.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#if defined MICROROS_USE_SERIAL

#define DBG_SECTION_NAME  "micro_ros_serial"
#define DBG_LEVEL         DBG_LOG
#include <rtdbg.h>


static int sem_initialized = 0;
static struct rt_semaphore rx_sem;
static rt_device_t micro_ros_serial;

 #define micro_rollover_useconds 4294967295

int clock_gettime(clockid_t unused, struct timespec *tp)
{
    (void)unused;
    static uint32_t rollover = 0;
    static uint32_t last_measure = 0;

    uint32_t m = rt_tick_get() * 1000 / RT_TICK_PER_SECOND * 1000;

    rollover += (m < last_measure) ? 1 : 0;

    rt_uint64_t real_us = (rt_uint64_t) (m + rollover * micro_rollover_useconds);

    tp->tv_sec = real_us / 1000000;
    tp->tv_nsec = (real_us % 1000000) * 1000;
    last_measure = m;
    return 0;
}

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

bool rtt_transport_open(struct uxrCustomTransport * transport)
{
    micro_ros_serial = rt_device_find(MICROROS_DEVIVE);
    if (!micro_ros_serial)
    {
        LOG_E("Failed to open device %s", MICROROS_DEVIVE);
        return 0;
    }
    LOG_E("succeed to open device %s", MICROROS_DEVIVE);
    if(sem_initialized == 0)
    {
        rt_sem_init(&rx_sem, "micro_ros_rx_sem", 0, RT_IPC_FLAG_FIFO);
        sem_initialized = 1;
    }
    rt_device_open(micro_ros_serial, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(micro_ros_serial, uart_input);
    return 1;
}

bool rtt_transport_close(struct uxrCustomTransport * transport)
{
    rt_device_close(micro_ros_serial);
    rt_sem_detach(&rx_sem);
    sem_initialized = 0;
    return 1;
}

size_t rtt_transport_write(struct uxrCustomTransport * transport, const uint8_t *buf, size_t len, uint8_t *errcode)
{
    return rt_device_write(micro_ros_serial, 0, buf, len);
}

size_t rtt_transport_read(struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode)
{
    int tick = rt_tick_get();
    for (int i = 0; i < len; ++i)
    {
        if(sem_initialized == 0)
        {
            rt_sem_init(&rx_sem, "micro_ros_rx_sem", 0, RT_IPC_FLAG_FIFO);
            sem_initialized = 1;
        }
        while (rt_device_read(micro_ros_serial, -1, &buf[i], 1) != 1)
        {
            rt_sem_take(&rx_sem, timeout / 4);
            if( (rt_tick_get() - tick) > timeout )
            {
                // LOG_E("Read timeout");
                return i;
            }
        }
    }
    // rt_kprintf("len = %d\t", len);
    return len;
}

#endif // MICRO_ROS_USE_SERIAL
