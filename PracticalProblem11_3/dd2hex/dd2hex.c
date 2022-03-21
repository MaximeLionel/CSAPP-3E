/*
Practice Problem 11.3 (solution page 1003)
    Write a program dd2hex.c that converts its 16-bit network byte order to a 32-bit
    hex number and prints the result. 
    
    For example,
        linux> ./dd2hex 128.2.194.242
        0x8002c2f2
*/

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

    inet_pton(AF_INET,argv[1],&network_ip);
    host_ip = ntohl(network_ip.s_addr);

    printf("host_ip: %x\n",host_ip);
   
    return 0;
}