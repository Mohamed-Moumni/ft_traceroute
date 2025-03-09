#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    char hostname[256];
    
    // Get the hostname of the current machine
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return 1;
    }

    // Use getaddrinfo to resolve the hostname to an IP address
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4

    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    // Convert the IP address to a string and print it
    char ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &ipv4->sin_addr, ip_str, sizeof(ip_str));
    
    printf("IP address of the host '%s': %s\n", hostname, ip_str);

    // Clean up
    freeaddrinfo(res);
    return 0;
}
