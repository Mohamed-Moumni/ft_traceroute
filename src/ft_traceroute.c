#include "ft_traceroute.h"

traceroute      *traceroute_setup(const char *hostname)
{
    dest_sock           dest_sock = {0};
    traceroute          *trace = NULL;
    int                 trace_socket;
    struct sockaddr_in  server_addr;
    int one = 1;

    dest_sock = get_sock_addr(hostname);
    trace = (struct traceroute_struct *)malloc(sizeof(struct traceroute_struct));
    if (!trace)
        print_error("Malloc: Memory Allocation Error");
    trace->destina_socket = dest_sock;
    trace_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (trace_socket == -1)
    {
        free(trace);
        print_error("Socket Creation Error");
    }
    trace->sock = trace_socket;
    int sock_opt = setsockopt(trace->sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));
    if (sock_opt < 0)
    {
        perror("Sockopt: ");
        print_error("setsockopt: set socket Option Error");
    }
    int raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (raw_sock < 0)
    {
        perror("Raw Socket: ");
    }
    trace->raw_sock = raw_sock;
    return trace;
}

void        send_probe_packet(int ttl, unsigned short int  flag, traceroute *trace, probe_packet *probe_pack)
{
    char send_buffer[BUFFER];
    probe_pack->ip_header.iph_ttl = ttl;
    probe_pack->ip_header.iph_len = sizeof(probe_pack);
    *((int *)probe_pack->data + 4) = flag;
    probe_pack->ip_header.iph_chksum = calculate_checksum(probe_pack, sizeof(probe_pack));

    memset(send_buffer, 0, BUFFER);
    memcpy(send_buffer, probe_pack, BUFFER);
    int send_res = sendto(trace->sock, send_buffer, BUFFER, 0, trace->destina_socket.dest_addr, trace->destina_socket.addr_len);
    if (send_res < 0)
    {
        perror("SendTo error: ");
        // print_error("Sendto: Sending Packet Error");   
    }
    printf("Packet Sent\n");
}

void        receive_probe_packet(traceroute *trace, probe_packet *probe_pack)
{
    printf("Recv Packet\n");
    char recv_buffer[BUFFER];
    ip_header *ip_head;
    dest_sock recv_dest;
    unsigned char *payload;

    int receive_res = recvfrom(trace->raw_sock, recv_buffer, BUFFER, 0, recv_dest.dest_addr, &recv_dest.addr_len);
    if (receive_res < 0)
        print_error("RecvFrom: Receive Packet Error");

    ip_head = (ip_header *)recv_buffer;
    payload = (unsigned char *)(recv_buffer + 20);
    int traceroute_id = *((int *)payload);
    int flag = *((unsigned short int *)(payload + 4));

    printf("___________________________ IP HEADER _________________________\n");
    printf("IP Header Fields:\n");
    printf("Version: %u\n", ip_head->iph_ver);
    printf("Header Length: %u (in words, so %u bytes)\n", ip_head->iph_ihl, ip_head->iph_ihl * 4);
    printf("Type of Service: %u\n", ip_head->iph_tos);
    printf("Total Length: %u\n", ip_head->iph_len);
    printf("Identification: %u\n", ip_head->iph_ident);
    printf("Flags: %u\n", ip_head->iph_flag);
    printf("Fragment Offset: %u\n", ip_head->iph_offset);
    printf("Time to Live: %u\n", ip_head->iph_ttl);
    printf("Protocol: %u\n", ip_head->iph_protocol);
    printf("Checksum: 0x%04X\n", ip_head->iph_chksum);
    printf("Source IP: %u.%u.%u.%u\n",
           (ip_head->iph_sourceip >> 24) & 0xFF,
           (ip_head->iph_sourceip >> 16) & 0xFF,
           (ip_head->iph_sourceip >> 8) & 0xFF,
           ip_head->iph_sourceip & 0xFF);
    printf("Destination IP: %u.%u.%u.%u\n",
           (ip_head->iph_destip >> 24) & 0xFF,
           (ip_head->iph_destip >> 16) & 0xFF,
           (ip_head->iph_destip >> 8) & 0xFF,
           ip_head->iph_destip & 0xFF);
    printf("___________________________Payload ________________________\n");
    printf("PID: %d\n", traceroute_id);
    printf("FLAG: %d\n", flag);
}