#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

void sendMsg(int fd);
void *recvMsg(int fd);
int errexit(const char *format, ...);
int connectTCP(const char *host, int port);

#define LINELEN 256

int main(int argc, char *argv[])
{
    char *host = "localhost";
    int port = 3344;
    pthread_t th;
    int sock ;

    switch (argc)
    {
    case 1:
        host = "localhost";
        break;
    case 3:
        port = atoi(argv[2]);
    case 2:
        host = argv[1];
        break;

    default:
        fprintf(stderr, "usage: TCPecho[host[port]]\n");
        exit(1);
    }
    sock = connectTCP(host, port);
    printf("已连接到服务器，fd：%d\n",sock);
    //启动接收线程
    if (pthread_create(&th, NULL,(void *(*)(void *)) recvMsg, (void *)sock) < 0)
        errexit("phread_create:%s\n", strerror(errno));

    printf("已启动读取线程\n");
    //启动发送线程
    // if (pthread_create(&th, NULL, (void *(*)(void *))sendMsg,
    //                    (void *)sock) < 0)
    //     errexit("phread_create:%s\n", strerror(errno));

    //循环处理输入字符
    int outchars;
    char buf[LINELEN + 1];

     while (1)
    {
        if(sock==NULL)
            errexit("与服务器断开连接：%s\n",strerror(errno)) ;
        while (fgets(buf, sizeof(buf), stdin))
        {
            buf[LINELEN] = '\0';
            int cc=sizeof buf;
            //printf("要发送的数据大小：%d\n",cc);
            if(write(sock, buf, sizeof buf)<0){
                printf("发送信息出错\n");
                errexit("send:%s\n", strerror(errno));
            }
            printf("--------\n发送完成\n\n");
            fflush(stdout);
        }
    }

    printf("回到主函数\n");
    exit(0);
}

void *recvMsg(int fd)
{
    int cc=0;
    char buf[LINELEN + 1];
    printf("进入接收线程，fd：%d\n",fd);
    while (1)
    {
        //printf("进入读取循环\n");
        memset(buf,'\0',sizeof(buf));
        while (cc = read(fd, buf, sizeof buf))
        {
            //printf("读取到的数据大小：%d\n",cc);
            if (cc < 0){
                errexit("recv:%s\n", strerror(errno));
                printf("接收信息出错\n");
                break;
            }
            //fflush(stdin);
            //fputs(buf, stdout);
            printf("%s",buf);
            fflush(stdout);
        }
    }
    printf("退出接收线程");
}

void sendMsg(int fd)
{
    int outchars;
    char buf[LINELEN + 1];

    printf("进入发送线程\n");
    while (1)
    {
        while (fgets(buf, sizeof buf, stdin))
        {
            buf[LINELEN] = '\0';
            outchars = strlen(buf);
            if(write(fd, buf, outchars)<0){
                printf("发送信息出错\n");
                errexit("send:%s\n", strerror(errno));
                break;
            }
        }
    }
    printf("退出发送线程\n");
}
