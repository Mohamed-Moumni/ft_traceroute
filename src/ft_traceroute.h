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

#define MAX_HOPS 64

typedef struct destination_sock
{
    struct sockaddr *dest_addr;
    socklen_t addr_len;
} dest_sock;

typedef struct traceroute_struct
{
    int sock;
    dest_sock destina_socket;
    unsigned int source_ip;
    unsigned int destination_ip;
} traceroute;

struct ipheader {
    unsigned char      iph_ihl:4, iph_ver:4; // IP header length and version
    unsigned char      iph_tos;              // Type of service
    unsigned short int iph_len;              // IP packet length (data + header)
    unsigned short int iph_ident;            // Identification
    unsigned short int iph_flag:3, iph_offset:13; // Flags and fragment offset
    unsigned char      iph_ttl;              // Time to live
    unsigned char      iph_protocol;         // Protocol type
    unsigned short int iph_chksum;           // IP datagram checksum
    unsigned int       iph_sourceip;         // Source IP address
    unsigned int       iph_destip;           // Destination IP address
};

void        print_error(const char *format, ...);
void        print_help(void);
dest_sock   get_sock_addr(const char *target_host);
void        socket_init(void);
void        traceroute_loop(traceroute *trace);
traceroute  *traceroute_setup(const char * hostname);
void        send_probe_packet(int ttl, int flag, traceroute *_trace);
char        *get_host_name(void);
#endif