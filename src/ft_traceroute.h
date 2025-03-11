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

#define MAX_HOPS    64
#define BUFFER      31

typedef struct destination_sock
{
    struct sockaddr *dest_addr;
    socklen_t addr_len;
} dest_sock;

typedef struct traceroute_struct
{
    int         sock;
    dest_sock   destina_socket;
    int         raw_sock;
} traceroute;

typedef struct ipheader {
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
}ip_header;

typedef struct probe_packet_struct {
    struct ipheader ip_header;
    unsigned char   data[6];
}probe_packet;

void            print_error(const char *format, ...);
void            print_help(void);
dest_sock       get_sock_addr(const char *target_host);
void            socket_init(void);
void            traceroute_loop(traceroute *trace);
traceroute      *traceroute_setup(const char * hostname);
probe_packet    *setup_probe_packet(traceroute *trace);
void            send_probe_packet(int ttl,  unsigned short int  flag, traceroute *trace, probe_packet *probe_pack);
unsigned short  calculate_checksum(void *b, int len);
void        receive_probe_packet(traceroute *trace, probe_packet *probe_pack);

#endif