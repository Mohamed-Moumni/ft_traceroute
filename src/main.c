#include "ft_traceroute.h"

int main(int ac, const char *av[])
{
    traceroute      *trace = NULL;
    if (ac != 2)
    {
        print_error("Invalid arguments");
        print_help();
    }
    if (strlen(av[1]) == 6 && !strcmp(av[1], "--help"))
        print_help();
    trace = traceroute_setup(av[1]);
    ft_traceroute(trace);
    return (0);
}