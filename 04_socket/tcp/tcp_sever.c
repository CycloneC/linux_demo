#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define PORT_SEVER      8888

int main(int argc, char const *argv[])
{
    int ret, fd_sever, fd_client, client_num = 0, addr_len = sizeof(struct sockaddr);
    struct sockaddr_in addr_sever; //struct sockaddr不用，而用struct sockaddr_in，比较好对元素赋值
    struct sockaddr_in addr_client;

    signal(SIGCHLD, SIG_IGN); //处理子进程的僵尸进程

    fd_sever = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd_sever)
    {
        printf("socket err\n");
        return -1;
    }

    memset(&addr_sever, 0, sizeof(addr_sever));
    addr_sever.sin_family      = AF_INET;
    addr_sever.sin_port        = htons(PORT_SEVER);
    addr_sever.sin_addr.s_addr = INADDR_ANY;

    ret = bind(fd_sever, (struct sockaddr *)&addr_sever, sizeof(struct sockaddr));
    if (-1 == ret)
    {
        printf("bind err\n");
        return -1;
    }

    ret = listen(fd_sever, 10); //允许的最多连接数设置为10
    if (-1 == ret)
    {
        printf("listen err\n");
        return -1;
    }

    while (1)
    {
        fd_client = accept(fd_sever, (struct sockaddr *)&addr_client, &addr_len); //阻塞等待
        if (-1 == ret)
        {
            printf("accept err\n");
            continue;
        }

        client_num++;
        printf("get connect from client %d : %s\n", client_num, inet_ntoa(addr_client.sin_addr));
        
        //创建子进程
        if (!fork())
        {
            /* 子进程运行*/
            char buf[1024] = {0};
            int recv_len = 0;

            while (1)
            {
                recv_len = recv(fd_client, buf, 1024, 0); //阻塞等待
                if (recv_len <= 0)
                {
                    close(fd_client);
                    return -1;
                }

                buf[recv_len] = '\0';
                printf("get data from client %d: %s\n", client_num, buf);
            }
        }
    }
    
    close(fd_sever);
    return 0;
}
