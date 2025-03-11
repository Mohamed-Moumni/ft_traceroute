#include "ft_traceroute.h"

void print_error(const char *format, ...)
{
    va_list args;
    fprintf(stderr, "ft_traceroute - error: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void print_help(void)
{
    printf("Version Rigorous traceroute\n");
    printf("Usage: ft_traceroute host");
    exit(1);
}

dest_sock get_sock_addr(const char *target_host)
{
    struct addrinfo *result;
    struct addrinfo hints;
    struct addrinfo *rp;
    dest_sock dest_sock_addr = {0};

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;

    int s = getaddrinfo(target_host, NULL, &hints, &result);
    if (s != 0)
        print_error("unknown host %s", target_host);
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        if (rp->ai_family == AF_INET && rp->ai_addr)
        {
            dest_sock_addr.dest_addr = (struct sockaddr *)malloc(sizeof(struct sockaddr));
            if (!dest_sock_addr.dest_addr)
                print_error("Malloc: Memory Allocation Error");
            memcpy(dest_sock_addr.dest_addr, rp->ai_addr, rp->ai_addrlen);
            struct sockaddr_in * dest = (struct sockaddr_in *)rp->ai_addr;
            dest->sin_port = htons(33455);
            dest_sock_addr.addr_len = rp->ai_addrlen;
            freeaddrinfo(result);
            return dest_sock_addr;
        }
    }
    freeaddrinfo(result);
    print_error("unknown host %s", target_host);
    return dest_sock_addr;
}

// void traceroute_loop(traceroute *trace, )
// {
//     for (int ttl = 1; ttl <= MAX_HOPS; ttl++)
//     {
//         for (int flag = 1; flag <= 3; flag++)
//         {
//             send_probe_packet(ttl, flag, trace);
//         }
//     }
// }

probe_packet    *setup_probe_packet(traceroute *trace)
{
    probe_packet    *probe_pack = {0};
    int             traceroute_id;

    probe_pack = (struct probe_packet_struct *)malloc(sizeof(probe_packet));
    if (!probe_pack)
        print_error("Malloc: Memory Allocation Error");
    
    traceroute_id = getpid();
    probe_pack->ip_header.iph_ihl = 5;
    probe_pack->ip_header.iph_ver = 4;
    probe_pack->ip_header.iph_tos = 0;
    probe_pack->ip_header.iph_len = 0;
    probe_pack->ip_header.iph_ident = htons(54321);
    probe_pack->ip_header.iph_flag = 0;
    probe_pack->ip_header.iph_offset = 0;
    probe_pack->ip_header.iph_chksum = 0;
    probe_pack->ip_header.iph_protocol = IPPROTO_UDP;
    probe_pack->ip_header.iph_sourceip = 0;

    // set the pid of the program as an identifier of traceroute
    *((int *)probe_pack->data) = traceroute_id;
    return probe_pack;
}

unsigned short int calculate_checksum(void *b, int len)
{
    unsigned short int *buf = b;
    unsigned int sum = 0;

    for (; len > 1; len -= 2)
        sum += *buf++;

    if (len == 1)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return ~sum;
}
