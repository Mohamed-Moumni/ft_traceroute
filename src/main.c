#include "ft_traceroute.h"

int main(int ac, const char *av[])
{
    traceroute *trace = NULL;
    if (ac != 2)
    {
        print_error("Invalid arguments");
        print_help();
    }
    // check the args input if it is --help or if its a host name
    if (strlen(av[1]) == 6 && !strcmp(av[1], "--help"))
        print_help();
    trace = traceroute_setup(av[1]);
    traceroute_loop(trace);
    return (0);
}