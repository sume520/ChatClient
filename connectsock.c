#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

int errexit(const char *format, ...);

int connectsock(const char* host,int port, const char* transport)
{
    struct hostent      *phe;
    struct servent      *pse;
    struct protoent     *ppe;
    struct sockaddr_in  sin;
    int s,type;

    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;

    // char *service="echo";
    // if(pse=getservbyname(service,transport))
    //     sin.sin_port=pse->s_port;
    // else if((sin.sin_port=htons((unsigned short)atoi(service)))==0)
    //     errexit("can't get \"%s\" service entry\n",service);


    sin.sin_port=port;

    if(phe=gethostbyname(host))
        memcpy(&sin.sin_addr,phe->h_addr,phe->h_length);
    else if((sin.sin_addr.s_addr=inet_addr(host))==INADDR_NONE)
        errexit("can't get \"%s\" host entry\n",host);

    if((ppe=getprotobyname(transport))==0)
        errexit("cant't get \"%s\" protocol entry\n",transport);

    if(strcmp(transport,"udp")==0)
        type=SOCK_DGRAM;
    else
        type=SOCK_STREAM;

    s=socket(PF_INET,type,ppe->p_proto);
    if(s<0)
        errexit("can't create socket: %s\n",strerror(errno));

    if(connect(s,(struct sockaddr *)&sin,sizeof(sin))<0)
        errexit("cant't connect to %s.%d: %ss\n",host,port,strerror(errno));
    
    return s;
}


int connectTCP(const char *host,int port)
{
    return connectsock(host,port,"tcp");
}
