#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>


/*返回str2第一次出现在str1中的位置(下表索引),不存在返回-1*/
int indexOf(char *str1,char *str2)
{
    char *p=str1;
    int i=0;
    p=strstr(str1,str2);
    if(p==NULL){
        return -1;
    }
    else{
        while(str1!=p)
        {
            str1++;
            i++;
        }
    }
    return i;
}

int post(char *ip,int port,char *page,char *msg,char[] recvline,int lianwangzhong){
    int sockfd,n;

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
        return -1;
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if(inet_pton(AF_INET,ip,&servaddr.sin_addr) <= 0){
        printf("transfer ip err");
        printf("ip---%s\n",ip);
        return -1;
    }

    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        printf("connect error\n");
        return -1;
    }
    lianwangzhong=1;

    write(sockfd,content,strlen(content));
    n = read(sockfd,recvline,1024);
    printf("n---%d\n",n);

    int ind = indexOf(recvline,"config");
    printf("in---%d\n",in);

    if(n < 0){
        printf("read error\n");
        return -1;
    }
    return 0;
}
int main()
{
    char msg[] = "Ceshilianwang=ceshi";
    char ip[] = "192.168.0.102";
    int port = 8989;
    char page[] = "sn/lianwang";
    int lianwangzhong = 0;
    char recvline[1024];
    int cg = post(ip,port,page,msg,recvline,lianwangzhong);
    if (cg){
        printf("lianwangzhong---%d\n",lianwangzhong);
        printf("recvline---%s\n",recvline);
    }
    exit(0);
}
