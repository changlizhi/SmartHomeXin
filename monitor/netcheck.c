#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netdb.h>
#include "include/monitor/alarm.h"

static struct addrinfo *get_host_addrinfo(const char *host, int port)
{
    struct addrinfo hints, *pAI;
    char service[16];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // don't care ipv4 or ipv6    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(service, "%d", port);
    if (getaddrinfo(host, service, &hints, &pAI))
        pAI = NULL;    return pAI;
    }
static int connect_to_addr(struct addrinfo *pAI, int *pSock)
{
    struct addrinfo *p;
    for (p = pAI; p != NULL; p = p->ai_next)
    {
        *pSock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (*pSock != -1)
        {
            if (connect(*pSock, p->ai_addr, p->ai_addrlen))
                close(*pSock);
            else
                break;
        }
    }
    return p == NULL;
}
static int connect_to_host(const char *host, int port)
{
    struct addrinfo *pAI;
    int sock = -1;
    if ((pAI = get_host_addrinfo(host, port)))
    {
        if (connect_to_addr(pAI, &sock))
            sock = -1;
        freeaddrinfo(pAI);
    }
    return sock;
}
int check_connection(const char *host)
{
    if (!host)
        host = "www.baidu.com";
    int sock = connect_to_host(host, 80);
    if (sock != -1)
        close(sock);
    return sock != -1;
}
