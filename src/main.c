#include "ft_traceroute.h"

int main(int ac, const char *av[])
{
    traceroute      *trace = NULL;
    probe_packet    *probe_pack = NULL;
    if (ac != 2)
    {
        print_error("Invalid arguments");
        print_help();
    }
    if (strlen(av[1]) == 6 && !strcmp(av[1], "--help"))
        print_help();
    trace = traceroute_setup(av[1]);
    probe_pack = setup_probe_packet(trace);
    // traceroute_loop(trace);
    send_probe_packet(4, 1, trace, probe_pack);
    receive_probe_packet(trace, probe_pack);
    return (0);
}