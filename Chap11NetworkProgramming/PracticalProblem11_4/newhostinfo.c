/*
Practice Problem 11.4 (solution page 1004)
The getaddrinfo and getnameinfo functions subsume the functionality of inet_
pton and inet_ntop, respectively, and they provide a higher-level of abstraction
that is independent of any particular address format. To convince yourself how
handy this is, write a version of hostinfo (Figure 11.17) that uses inet_ntop in-
stead of getnameinfo to convert each socket address to a dotted-decimal address
string.
*/

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
    memset(buff,0,MAX_LINE);

    /* By argv[1] = host name, get the addrinfo(listp, ipv4) */
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family     = AF_INET;      // IPv4 only
    hints.ai_socktype   = SOCK_STREAM;  // At most one addrinfo structure
    if((rc=getaddrinfo(argv[1], NULL, &hints, &listp)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(1);
    }

    int i = 0;
    for(p = listp; p; p=p->ai_next)
    {
        printf("%d ",i);
        // struct sockaddr_in is a type of struct sockaddr.
        struct sockaddr_in* p_sa_in = (struct sockaddr_in*)p->ai_addr;  
        inet_ntop(AF_INET,&(p_sa_in->sin_addr),buff,MAX_LINE);
        printf("%s\n",buff);
        memset(buff,0,MAX_LINE);
        i++;
    }
    
    /* Clean up */
    freeaddrinfo(listp);

    exit(0);
}