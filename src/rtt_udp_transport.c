#include <rtthread.h>

#if defined MICRO_ROS_USE_UDP

#include "micro_ros_rtt.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>

#define DBG_SECTION_NAME  "micro_ros_udp"
#define DBG_LEVEL         DBG_LOG
#include <rtdbg.h>

int sock;
struct hostent *host;
struct sockaddr_in server_addr;

#define micro_rollover_useconds 4294967295

int clock_gettime(clockid_t unused, struct timespec *tp)
{
    (void)unused;
    static uint32_t rollover = 0;
    static uint32_t last_measure = 0;

    uint64_t m = rt_tick_get() * 1000 / RT_TICK_PER_SECOND * 1000;

    rollover += (m < last_measure) ? 1 : 0;

    rt_uint64_t real_us = (rt_uint64_t) (m + rollover * micro_rollover_useconds);

    tp->tv_sec = real_us / 1000000;
    tp->tv_nsec = (real_us % 1000000) * 1000;
    last_measure = m;
    return 0;
}

bool rtt_udp_transport_open(struct uxrCustomTransport * transport)
{
    struct micro_ros_agent_locator* locator = (struct micro_ros_agent_locator *) transport->args;

    host = (struct hostent *) gethostbyname( locator->address );

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        LOG_E("Socket error\n");
        closesocket(sock);
        return 0;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(locator->port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        rt_kprintf("Connect fail!\n", sock);
        return 0;
    }
     rt_kprintf("Connect sucessful!\n", sock);
    return 1;
}

bool rtt_udp_transport_close(struct uxrCustomTransport * transport)
{
    return closesocket(sock);;
}

size_t rtt_udp_transport_write(struct uxrCustomTransport * transport, const uint8_t *buf, size_t len, uint8_t *errcode)
{
    size_t rv = 0;
    ssize_t bytes_sent = send(sock, (void*)buf, len, 0);
    if (-1 != bytes_sent)
    {
        rv = (size_t)bytes_sent;
        *errcode = 0;
    }
    else
    {
        *errcode = 1;
    }
    return rv;
}

size_t rtt_udp_transport_read(struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode)
{
    size_t rv = 0;
    timeout = (timeout <= 0) ? 1 : timeout;
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    if (0 != setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)))
    {
        *errcode = 1;
        return 0;
    }
    ssize_t bytes_received = recv(sock, (void*)buf, len, 0);
    if (-1 != bytes_received)
    {
        rv = (size_t)bytes_received;
        *errcode = 0;
    }
    else
    {
        *errcode = 1;
    }

    return rv;
}

#endif // MICRO_ROS_USE_UDP
