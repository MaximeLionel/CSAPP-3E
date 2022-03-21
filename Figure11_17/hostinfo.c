#include <csapp.h>

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

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family     = AF_INET;      // IPv4 only
    hints.ai_socktype   = SOCK_STREAM;  // At most one addrinfo structure
    getaddrinfo(argv[1], )

}