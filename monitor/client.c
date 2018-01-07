#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/zifuchuan.h"

#define MAXLINE 1024

int post(char *ip,int port,char *page,char *msg){
    int sockfd,n;
    char recvline[MAXLINE];
    struct sockaddr_in servaddr;

    char content[4096];
    char content_page[50];
    sprintf(content_page,"POST /%s HTTP/1.1\r\n",page);
    char content_host[50];
    sprintf(content_host,"HOST: %s:%d\r\n",ip,port);

    char content_type[] = "Content-Type: application/x-www-form-urlencoded\r\n";
    char content_len[50];
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(msg));
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,msg);
    printf("content---%s\n",content);

    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        printf("sockfd2---%d\n",sockfd);
        printf("socket error\n");
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if(inet_pton(AF_INET,ip,&servaddr.sin_addr) <= 0){
        printf("ip---%s\n",ip);
    }

    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        printf("connect error\n");
    }

    write(sockfd,content,strlen(content));
    n = read(sockfd,recvline,MAXLINE);
    printf("recvline---%s\n",recvline);
    printf("n---%d\n",n);

    int ind = indexOf(recvline,"config");
    printf("ind---%d\n",ind);

    //if(fputs(recvline,stdout) == EOF){
    //    printf("fputs error\n");
    //}
    if(n < 0){
        printf("read error\n");
    }

}
int main()
{
    char msg[] = "Xuliehao=1234&Macdizhi=dd:dd:dd:aa:aa:aa";
    char ip[] = "192.168.0.102";
    int port = 8989;
    char page[] = "sn/jieshou";
    post(ip,port,page,msg);
    exit(0);
}
