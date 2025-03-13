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

double get_rtt_probe_packet(const struct timeval time)
{
    struct timeval  currentime;
    double    rtt;

    if (gettimeofday(&currentime, NULL) < 0)
    {
        perror("Get Time Of Day Failed: ");
        exit(1);
    }
    return ((currentime.tv_sec - time.tv_sec) * 1000 + (currentime.tv_usec - time.tv_usec) * 0.001);
}

void print_router(double rtt)
{
    printf("%.2f ms", rtt);
    fflush(stdout);
}
void print_trace(int ttl, const char *addr, double rtt, bool flushed)
{
    if (!flushed)
    {
        printf("%d  %s (%s) %.2f ms ", ttl, addr, addr, rtt);
    }
    else
    {
        printf("%.2f ms ", rtt);
    }
    fflush(stdout);
}