#include "ft_traceroute.h"

void ft_traceroute(traceroute *trace)
{
    int send_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (send_sock < 0)
    {
        perror("Send Socket: ");
        exit(1);
    }
    int binded = bind(send_sock, (struct sockaddr *)&trace->bind_sock.dest_adrr, trace->bind_sock.addr_len);
    if (binded < 0)
    {
        perror("bind: ");
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
    trace->src_port = getpid();
    trace->destina_addr = dest_addr;
    trace->bind_sock.addr_len = dest_addr.addr_len;
    trace->bind_sock.dest_adrr.sin_family = dest_addr.dest_addr->sin_family;
    trace->bind_sock.dest_adrr.sin_port = htons(trace->src_port);
    return trace;
}

int recv_packet(traceroute *trace, int ppacket, int counter)
{
    char                recv_buffer[BUFFER];
    struct ip           *ip_head, *orig_ip_header;
    struct icmp         *icmp_header;
    int                 ip_header_len, icmp_data_offset, orig_ip_header_len;
    struct sockaddr_in  recv_addr;
    socklen_t           addr_len;
    char                *ip_addr;

    int receive_res = recvfrom(trace->recv_sock, recv_buffer, BUFFER, 0, (struct sockaddr *)&recv_addr, &addr_len);
    if (receive_res < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            printf("* ");
            return (1);
        }
        else
        {
            perror("recvfrom :");
            exit(1);
        }
    }

    ip_head = (struct ip *)recv_buffer;
    ip_header_len = ip_head->ip_hl * 4;
    icmp_header = (struct icmp *)(recv_buffer + ip_header_len);
    icmp_data_offset = ip_header_len + 8;
    orig_ip_header = (struct ip *)(recv_buffer + icmp_data_offset);
    orig_ip_header_len = orig_ip_header->ip_hl * 4;
    struct udphdr *udp_header = (struct udphdr *)(recv_buffer + icmp_data_offset + orig_ip_header_len);

    if (icmp_header->icmp_type == ICMP_TIMXCEED || icmp_header->icmp_type == ICMP_UNREACH_PORT)
    {
        if (htons(udp_header->uh_sport) == trace->src_port && (htons(udp_header->uh_dport) - counter) == DEST_PORT)
        {
            ip_addr = inet_ntoa(ip_head->ip_src);
            if (ppacket && strcmp(ip_addr, last_addr_host))
            {
                printf("\n");
                printf("    ");
                print_router_ip(ip_head, ip_addr);
                memcpy(last_addr_host, ip_addr, strlen(ip_addr));
            }
            else if (ppacket == 0)
            {
                print_router_ip(ip_head, ip_addr);
                memcpy(last_addr_host, ip_addr, strlen(ip_addr));
            }
            if (icmp_header->icmp_type == ICMP_UNREACH_PORT && ppacket == 2)
            {
                return (2);
            }
        }
    }
    return (0);

}

void traceroute_loop(traceroute *trace)
{
    struct timeval start_time;
    struct timeval timeout = {TIMEOUT, 0};
    int counter;

    counter = 0;
    if (setsockopt(trace->recv_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt :");
        exit(1);
    }
    for (int ttl = 1; ttl <= MAX_HOPS; ttl++)
    {
        printf(" %d  ", ttl);
        memset(last_addr_host, 0, BUFFER);
        if (setsockopt(trace->send_sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
            perror ("setSocketOpt: ");
            exit(1);
        }
        for (int ppacket=0; ppacket < PROBE_PACK; ppacket++)
        {
            trace->destina_addr.dest_addr->sin_port = htons(DEST_PORT + counter);
            gettimeofday(&start_time, NULL);
            int send_res = sendto(trace->send_sock, NULL, 0, 0, (struct sockaddr *)trace->destina_addr.dest_addr, trace->destina_addr.addr_len);
            if (send_res < 0)
            {
                perror("SendTo : ");
                exit(1);
            }
            int rec = recv_packet(trace, ppacket, counter);
            print_rtt(&start_time);
            if (rec == 2)
            {
                printf("\n");
                exit(0);
            }
            counter++;
        }
        printf("\n");
        memset(last_addr_host, 0, BUFFER);
    }
}
