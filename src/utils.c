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
            dest_sock_addr.addr_len = rp->ai_addrlen;
            freeaddrinfo(result);
            return dest_sock_addr;
        }
    }
    freeaddrinfo(result);
    print_error("unknown host %s", target_host);
    return dest_sock_addr;
}

void        traceroute_loop(traceroute *trace)
{
    for (int ttl = 1; ttl <= MAX_HOPS; ttl++){
        for (int flag = 1; flag <= 3; flag++)
        {
            send_probe_packet(ttl, flag, trace);
        }
    }
}

char        *get_host_name(void)
{
    char *hostname = NULL;

    hostname = (char *)malloc(sizeof(char) * 256);
    if (!hostname)
        print_error("Malloc: Allocation Memory error");
}