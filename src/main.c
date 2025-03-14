#include "ft_traceroute.h"

int main(int ac, const char *av[])
{
    traceroute *trace = NULL;

    if (ac != 2)
    {
        print_error("Invalid arguments");
        print_help();
    }
    if (strlen(av[1]) == 6 && !strcmp(av[1], "--help"))
        print_help();
    trace = traceroute_setup(av[1]);
    printf("ft_traceroute to %s (%s), %d hops max, 40 byte packets\n", av[1], inet_ntoa(trace->destina_addr.dest_addr->sin_addr), MAX_HOPS);
    ft_traceroute(trace);
    traceroute_loop(trace);
    return (0);
}