#include "ft_traceroute.h"


void ft_traceroute(traceroute *trace)
{
    // create the datagram socket for sending that uses udp
    // create the raw socket for the receiving that uses icmp
    // call a method that send and receive via a loop

    int send_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (send_sock < 0)
    {
        perror("Send Socket: ");
        exit(1);
    }
    trace->send_sock = send_sock;
    int recv_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (recv_sock < 0)
    {
        perror("Recv Socket: ");
        exit(1);
    }
    trace->recv_sock = recv_sock;
}

traceroute      *traceroute_setup(const char *hostname)
{
    dest_sock           dest_addr = {0};
    traceroute          *trace = NULL;

    dest_addr = get_sock_addr(hostname);
    trace = (struct traceroute_struct *)malloc(sizeof(struct traceroute_struct));
    if (!trace)
        print_error("Malloc: Memory Allocation Error");
    trace->destina_addr = dest_addr;
    return trace;
}

void traceroute_loop(traceroute *trace)
{
    char send_buffer[BUFFER];
    char recv_buffer[BUFFER];
    struct ip *ip_head;
    struct icmp * payload;
    struct sockaddr_in recv_addr;
    socklen_t addr_len;

    for (int ttl = 1; ttl <= MAX_HOPS; ttl++)
    {
        printf("--------------------TTL-----------------------\n");
        printf("ttl: %d\n", ttl);
        memset(send_buffer, 0, BUFFER);
        if (setsockopt(trace->send_sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
            perror ("ping: can't set unicast time-to-live");
            exit(2);
        }
        int send_res = sendto(trace->send_sock, send_buffer, BUFFER, 0, (struct sockaddr *)trace->destina_addr.dest_addr, trace->destina_addr.addr_len);
        if (send_res < 0)
        {
            perror("SendTo error: ");
            exit(1);
        }
    
        struct timeval timeout = {TIMEOUT, 0};
        if (setsockopt(trace->recv_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            perror("setsockopt :");
        }
        int receive_res = recvfrom(trace->recv_sock, recv_buffer, BUFFER, 0, (struct sockaddr *)&recv_addr, &addr_len);
        if (receive_res < 0)
        {
            perror("recvfrom :");
        }

        ip_head = (struct ip *)recv_buffer;
        payload = (struct icmp *)(recv_buffer + ip_head->ip_hl * 4);
    
        printf("IP Header:\n");
        printf("  Version        : %d\n", ip_head->ip_v);
        printf("  Header Length  : %d bytes\n", ip_head->ip_hl * 4);
        printf("  Type of Service: %d\n", ip_head->ip_tos);
        printf("  Total Length   : %d\n", ntohs(ip_head->ip_len));
        printf("  Identification : %d\n", ntohs(ip_head->ip_id));
        printf("  Fragment Offset: %d\n", ntohs(ip_head->ip_off));
        printf("  Time to Live   : %d\n", ip_head->ip_ttl);
        printf("  Protocol       : %d\n", ip_head->ip_p);
        printf("  Header Checksum: %d\n", ntohs(ip_head->ip_sum));
        printf("  Source IP      : %s\n", inet_ntoa(ip_head->ip_src));
        printf("  Destination IP : %s\n", inet_ntoa(ip_head->ip_dst));
    
        printf("type: %d\n", payload->icmp_type);
        printf("code: %d\n", payload->icmp_code);
    }
}
