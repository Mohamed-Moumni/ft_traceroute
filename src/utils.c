#include "ft_traceroute.h"

char last_addr_host[BUFFER];

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
            dest_sock_addr.dest_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
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

void print_rtt(struct timeval *time_start)
{
    struct timeval  current_time;
    double          rtt;
    
    if (gettimeofday(&current_time, NULL) < 0)
    {
        perror("Get time of Day: ");
        exit(1);
    }
    rtt = ((current_time.tv_sec - time_start->tv_sec) * 1000.0 + (current_time.tv_usec - time_start->tv_usec) * 0.001);
    printf("%.2f ms ", rtt);
    fflush(stdout);
}

void print_router_ip(struct ip * ip_head, char *ip_addr)
{
    char                host[BUFFER];
    char                service[BUFFER];
    struct sockaddr_in  sa;
    
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = ip_head->ip_src.s_addr;
    sa.sin_port = 0;
    getnameinfo((struct sockaddr *)&sa, sizeof(sa), host, sizeof(host), service, sizeof(service),0);
    printf("%s (%s) ", host,ip_addr);
    fflush(stdout);
}