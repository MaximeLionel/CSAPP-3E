/*
* tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
*
GET method to serve static and dynamic content
*/

#include "csapp.h"

void doit(int fd);
void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
int get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);

int main(int argc, char** argv)
{
    int listenfd, connfd;
    char hostname[MAX_LINE],port[MAX_LINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    while(1)
    {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);
        getnameinfo((SA*)&clientaddr, clientlen, hostname, MAX_LINE, port, MAX_LINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
        doit(connfd);
        close(connfd);
    }

    
}


void doit(int fd)
{
    int is_static;
    struct stat sbuf;
    char buf[MAX_LINE], method[MAX_LINE], uri[MAX_LINE], version[MAX_LINE];
    char filename[MAX_LINE], cgiargs[MAX_LINE];
    rio_t rio;

    rio_readinitb(&rio, fd);    // associate descriptor fd with rio_t type address of rio.
    rio_readlineb(&rio, buf, MAX_LINE); // read from rio to buf
    printf("Request headers: \n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s",method,uri,version); // extract request line to method, uri and version
    if(strcasecmp(method, "GET"))
    {
        clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
        return;
    }
    read_requesthdrs(&rio);

    is_static = parse_uri(uri, filename, cgiargs);
    // If cannot find the file, then give 404 error and return.
    if(stat(filename, &sbuf) < 0) // get file info of filename to sbuf, if success, return 0. If fails, return -1.
    {
        clienterror(fd, filename, "404", "Not found", "Tiny couldn't find the file");
        return;
    }

    /* Serve Static Content */
    if(is_static)
    {
        // st_mode - protection
        // S_ISREG - is regular file
        // S_IRUSR - owner has read permission
        if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR & sbuf.st_mode))
        {
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
            return;
        }
        serve_static(fd,filename,sbuf.st_size);
    }  
    /* Serve Dynamic Content */
    else
    {
        // S_IXUSER - owner has execute permission
        if(!(S_ISREG(sbuf.st_mode))||!(S_IXUSR & sbuf.st_mode))
        {
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program.");
            return;
        }
        serve_dynamic(fd, filename, cgiargs);
    } 
}

/* 
When client provde the method that is not supported,
clienterror function build the HTTP response body and 
HTTP response. Then pass to 'fd'.
*/
void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg)
{
    char buf[MAX_LINE], body[MAXBUF];

    /* Build HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n",body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, cause);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print HTTP response */
    sprintf(buf, "HTTP/1,0 %s %s\r\n",errnum,shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

/* 
Tiny will not use any information from request headers.
Tiny simply reads and ignores them by calling read_requesthdrs.
*/
void read_requesthdrs(rio_t *rp)
{
    char buf[MAX_LINE];
    // read from rp (client) to buf with MAX_LINE-1 bytes
    Rio_readlineb(rp, buf, MAX_LINE); 
    // Read until end of headers
    while(strcmp(buf, "\r\n")) // if equal, strcmp return 0
    {
        Rio_readlineb(rp, buf, MAX_LINE);
        printf("%s", buf);
    }
    return;
}


/*
Go through uri.
Extract arguments to cgiargs.
Extract "." + directory to filename.

Return:
    1 - Static Content
    0 - Dynamic Content
*/
int parse_uri(char *uri, char *filename, char *cgiargs)
{
    char *ptr;

    /* Static content */
    if(!strstr(uri,"cgi-bin")) // strstr return a point if find "cgi-bin" in uri
    {
        strcpy(cgiargs, "");// copy "" to cgiargs. No arguments for Static content.
        strcpy(filename, ".");  // to build directory './'
        strcpy(filename, uri);
        if(uri[strlen(uri)-1]=='/')
        {
            strcat(filename, "home.html"); // build directory './home.html'
        }
        return 1;
    }
    /* Dynamic Content */
    else
    {
        ptr = index(uri, '?'); // find '?' address
        if(ptr) // if find '?' address
        {
            strcpy(cgiargs, ptr+1); // extract the arguments' string
            *ptr = '\0';
        }
        else    // // if not find '?' address
        {
            strcpy(cgiargs,""); // means no arguments available
        }
        strcpy(filename, ".");
        strcat(filename, uri);
        return 0;
    }
}

/*
    Construct Response headers and Response body to buf.
    Pass buf to fd.

    Open target file and pass file data to memory srcp.
    Pass srcp to fd.
*/
void serve_static(int fd, char *filename, int filesize)
{
    int srcfd;
    char *srcp, filetype[MAX_LINE], buf[MAXBUF];

    /* Send Response Headers to clients*/
    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n",buf);
    sprintf(buf, "%sConnection: close\r\n",buf);
    sprintf(buf, "%sContent-length: %d\r\n",buf,filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    Rio_writen(fd, buf, strlen(buf));   // Write in fd from buf
    printf("Response headers:\n");
    printf("%s",buf);

    /* Send Response Body to client */
    srcfd = Open(filename, O_RDONLY, 0);
    // maps srcfd file to memory and return pointer
    srcp = Mmap(0,filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Close(srcfd);
    // write to fd from srcp
    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize); // delete the mapping
}

/*
Get file type from filename to filetype.
*/
int get_filetype(char *filename, char *filetype)
{
    if(strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if(strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if(strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if(strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else 
        strcpy(filetype, "text/plain");
}

/*
Return 1st part of HTTP response
Fork a child process and execute the cgi program.
*/
void serve_dynamic(int fd, char *filename, char *cgiargs)
{
    char buf[MAX_LINE], *emptylist[]={NULL};

    /* Return first part of HTTP response */
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Go to Child Process */
    if(Fork()==0)// if fork return value ==0, means it's child process. If PID, means it's parent process.
    {
        setenv("QUERY_STRING",cgiargs,1);
        Dup2(fd, STDOUT_FILENO);    // Redirect stdout to fd
        Execve(filename, emptylist, environ); // Execute CGI program
    }
    Wait(NULL); // Parent process is waiting
}