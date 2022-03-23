/*
Practice Problem 11.2 (solution page 1003)
    Write a program hex2dd.c that converts its 16-bit hex argument to a 16-bit
    network byte order and prints the result. 
    
    For example
        linux> ./hex2dd 0x400
        ip address: 0.0.4.0S
*/

#include <stdio.h>
#include "csapp.h"
#define MAX_BUF 0x10

int main(int argc, char** argv)
{
    struct in_addr network_ip;  // Address in network byte order
    uint32_t host_ip;           // Address in host byte order
    char network_str_ip[MAX_BUF];        // String Address of network IP

    if(argc != 2)
    {
        printf("Wrong Input and will return!\n");
        return 0;
    }

    sscanf(argv[1], "%x", &host_ip);
    printf("host_ip: %d\n",host_ip);
    network_ip.s_addr = htonl(host_ip);

    inet_ntop(AF_INET, &network_ip, network_str_ip, MAX_BUF);
    printf("ip address: %s\n",network_str_ip);
    return 0;
}