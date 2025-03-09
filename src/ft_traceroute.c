#include "ft_traceroute.h"

traceroute      *traceroute_setup(const char *hostname)
{
    dest_sock           dest_sock = {0};
    traceroute          *trace = NULL;
    int                 trace_socket;
    struct sockaddr_in  server_addr;

    dest_sock = get_sock_addr(hostname);
    trace = (struct traceroute_struct *)malloc(sizeof(struct traceroute_struct));
    if (!trace)
        print_error("Malloc: Memory Allocation Error");
    
    trace_socket = socket(AF_INET, SOCK_RAW, 0);
    if (trace_socket == -1)
    {
        free(trace);
        print_error("Socket Creation Error");
    }
    trace->sock = trace_socket;
    return trace;
}

void        send_probe_packet(int ttl, int flag, traceroute *_trace)
{
    struct ipheader;

    // header.ip_hl = 5;  // Header length (in 32-bit words)
    // header.ip_v = 4;   // IPv4
    // header.ip_tos = 0; // Type of service
    // header.ip_len = sizeof(struct ip) + sizeof(payload); // Total length = IP header + payload
    // header.ip_id = htons(54321); // ID
    // header.ip_off = 0; // Fragment offset
    // header.ip_ttl = 255; // Time to live
    // header.ip_p = IPPROTO_RAW; // Protocol: Raw IP (no transport layer)
    // header.ip_sum = 0; // Checksum (auto-calculated)
    // header.ip_src.s_addr = inet_addr("192.168.0.1"); // Source IP
    // header.ip_dst.s_addr = inet_addr("192.168.0.2"); // Destination IP
    // ip_header.ip_hl = 5;
    // ip_header.ip_v = 4;
    // ip_header.ip_tos = 0;
    // ip_header.ip_len = sizeof(struct ip);
    // ip_header.ip_id = htons(54321);
    // ip_header.ip_off = 0;
    // ip_header.ip_ttl = ttl;
    // ip_header.ip_p = IPPROTO_UDP;
    // ip_header.ip_sum = 0;
    // ip_header.ip_src.s_addr =;
    // ip_header.ip_dst.s_addr = ;
}

void        receive_probe_packet(void);