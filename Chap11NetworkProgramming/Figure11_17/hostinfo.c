#include "csapp.h"

int main(int argc, char** argv)
{
    struct addrinfo *p, *listp, hints;
    char buff[MAX_LINE];
    int rc, flags;

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }

    /* By argv[1] = host name, get the addrinfo(listp, ipv4) */
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family     = AF_INET;      // IPv4 only
    hints.ai_socktype   = SOCK_STREAM;  // At most one addrinfo structure
    if((rc=getaddrinfo(argv[1], NULL, &hints, &listp)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(1);
    }

    /* Walk through addrinfo structure list and print IP */
    flags = NI_NUMERICHOST; // force getnameinfo to return a numeric address string as a domain name in host
    for(p = listp; p; p=p->ai_next)
    {
        getnameinfo(p->ai_addr,p->ai_addrlen,buff,MAX_LINE,NULL,0,flags);
        printf("%s\n",buff);
    }

    /* Clean up */
    freeaddrinfo(listp);

    exit(0);
}