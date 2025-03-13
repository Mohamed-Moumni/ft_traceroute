#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <stdbool.h>

#define BUFFER 1024
#define TIMEOUT 1
#define MAX_HOPS 30

typedef struct destination_sock
{
    struct sockaddr_in  *dest_addr;
    socklen_t           addr_len;
} dest_sock;

typedef struct traceroute_struct
{
    int send_sock;
    int recv_sock;
    dest_sock destina_addr;
    int dest_port;
} traceroute;

void print_error(const char *format, ...);
void print_help(void);
dest_sock get_sock_addr(const char *target_host);
void socket_init(void);
void traceroute_loop(traceroute *trace);
traceroute *traceroute_setup(const char *hostname);
unsigned short calculate_checksum(void *b, int len);
void ft_traceroute(traceroute *trace);
void traceroute_loop(traceroute *trace);
double get_rtt_probe_packet(const struct timeval time);
void print_trace(int ttl, const char *addr, double rtt, bool flushed);
// Troubleshooting

void print_ip_header_struct(struct ip *ip_head);

#endif