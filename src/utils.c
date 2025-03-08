#include "ft_traceroute.h"

void print_error(const char *err_msg)
{
    printf("ft_traceroute Error: %s\n", err_msg);
}

void print_help(void)
{
    printf("Version Rigorous traceroute\n");
    printf("Usage: ft_traceroute host");
    exit(1);
}