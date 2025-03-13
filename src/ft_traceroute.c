#include "ft_traceroute.h"

void ft_traceroute(traceroute *trace)
{
    // create the datagram socket for sending that uses udp
    // create the raw socket for the receiving that uses icmp
    // call a method that send and receive via a loop

    int send_sock = socket(AF_INET, SOCK_DGRAM, 0);
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
    traceroute_loop(trace);
}

traceroute *traceroute_setup(const char *hostname)
{
    dest_sock dest_addr = {0};
    traceroute *trace = NULL;

    dest_addr = get_sock_addr(hostname);
    trace = (struct traceroute_struct *)malloc(sizeof(struct traceroute_struct));
    if (!trace)
        print_error("Malloc: Memory Allocation Error");
    trace->destina_addr = dest_addr;
    trace->dest_port = getpid();
    return trace;
}

void traceroute_loop(traceroute *trace)
{
    char recv_buffer[BUFFER];
    struct ip *ip_head, *orig_ip_header;
    struct icmp *icmp_header;
    struct sockaddr_in recv_addr;
    socklen_t addr_len;
    struct timeval start_time;
    int ip_header_len, icmp_data_offset, orig_ip_header_len;
    double rtt;
    bool flushed;

    for (int ttl = 1; ttl <= MAX_HOPS; ttl++)
    {
        flushed = false;
        for (int probe_packet = 1; probe_packet <= 3; probe_packet++)
        {
            if (setsockopt(trace->send_sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
            {
                perror("ping: can't set unicast time-to-live");
                exit(2);
            }
            if (gettimeofday(&start_time, NULL) < 0) {
                perror("gettimeofday failed: ");
                exit(1);
            }

            trace->destina_addr.dest_addr->sin_family = AF_INET;
            trace->destina_addr.dest_addr->sin_port = htons(trace->dest_port);
            int send_res = sendto(trace->send_sock, NULL, 0, 0, (struct sockaddr *)trace->destina_addr.dest_addr, trace->destina_addr.addr_len);
            if (send_res < 0)
            {
                perror("SendTo error: ");
                exit(1);
            }

            struct timeval timeout = {TIMEOUT, 0};
            if (setsockopt(trace->recv_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
            {
                perror("setsockopt :");
            }
            int receive_res = recvfrom(trace->recv_sock, recv_buffer, BUFFER, 0, (struct sockaddr *)&recv_addr, &addr_len);
            if (receive_res < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // print_trace(ttl, inet_ntoa(ip_head->ip_src), rtt, flushed);
                    printf("*\n");
                }
                else
                {
                    perror("recvfrom :");
                }
            }

            ip_head = (struct ip *)recv_buffer;
            ip_header_len = ip_head->ip_hl * 4;
            icmp_header = (struct icmp *)(recv_buffer + ip_header_len);
            if (icmp_header->icmp_type == ICMP_TIMXCEED)
            {
                icmp_data_offset = ip_header_len + 8;
                orig_ip_header = (struct ip *)(recv_buffer + icmp_data_offset);
                orig_ip_header_len = orig_ip_header->ip_hl * 4;
                struct udphdr *udp_header = (struct udphdr *)(recv_buffer + icmp_data_offset + orig_ip_header_len);
                if (ntohs(udp_header->uh_dport) == trace->dest_port)
                {
                    rtt = get_rtt_probe_packet(start_time);
                }
                print_trace(ttl, inet_ntoa(ip_head->ip_src), rtt, flushed);   
            }
            else if (icmp_header->icmp_type == ICMP_UNREACH_PORT)
            {
                icmp_data_offset = ip_header_len + 8;
                orig_ip_header = (struct ip *)(recv_buffer + icmp_data_offset);
                orig_ip_header_len = orig_ip_header->ip_hl * 4;
    
                struct udphdr *udp_header = (struct udphdr *)(recv_buffer + icmp_data_offset + orig_ip_header_len);
                if (ntohs(udp_header->uh_dport) == trace->dest_port)
                {
                    rtt = get_rtt_probe_packet(start_time);
                }
                print_trace(ttl, inet_ntoa(ip_head->ip_src), rtt, flushed);
                exit(0);
            }
            flushed = true;
            // printf("IP Header:\n");
            // printf("  Version        : %d\n", ip_head->ip_v);
            // printf("  Header Length  : %d bytes\n", ip_head->ip_hl * 4);
            // printf("  Type of Service: %d\n", ip_head->ip_tos);
            // printf("  Total Length   : %d\n", ntohs(ip_head->ip_len));
            // printf("  Identification : %d\n", ntohs(ip_head->ip_id));
            // printf("  Fragment Offset: %d\n", ntohs(ip_head->ip_off));
            // printf("  Time to Live   : %d\n", ip_head->ip_ttl);
            // printf("  Protocol       : %d\n", ip_head->ip_p);
            // printf("  Header Checksum: %d\n", ntohs(ip_head->ip_sum));
            // printf("  Source IP      : %s\n", inet_ntoa(ip_head->ip_src));
            // printf("  Destination IP : %s\n", inet_ntoa(ip_head->ip_dst));
            
            // printf("type: %d\n", icmp_header->icmp_type);
            // printf("code: %d\n", icmp_header->icmp_code);
            // printf("UDP Source Port      : %d\n", ntohs(udp_header->uh_sport));
            // printf("UDP Destination Port : %d\n", ntohs(udp_header->uh_dport));
            // printf("UDP Length           : %d\n", ntohs(udp_header->uh_ulen));
            // printf("UDP Checksum         : %d\n", ntohs(udp_header->uh_sum));
        }
        printf("\n");
    }
}
