#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define PORT_SEVER      8888

int main(int argc, char const *argv[])
{
    int ret, len, fd, addr_len = sizeof(struct sockaddr);
    struct sockaddr_in addr_sever; //struct sockaddr不用，而用struct sockaddr_in，比较好对元素赋值
    struct sockaddr_in addr_client;
    char buf[1024] = {0};

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == fd)
    {
        printf("socket err\n");
        return -1;
    }

    memset(&addr_sever, 0, sizeof(addr_sever));
    addr_sever.sin_family      = AF_INET;
    addr_sever.sin_port        = htons(PORT_SEVER);
    addr_sever.sin_addr.s_addr = INADDR_ANY;

    ret = bind(fd, (struct sockaddr *)&addr_sever, sizeof(struct sockaddr));
    if (-1 == ret)
    {
        printf("bind err\n");
        return -1;
    }

    while (1)
    {
        len = recvfrom(fd, buf, 1024, 0, (struct sockaddr *)&addr_client, &addr_len);
        if (len > 0)
        {
            buf[len] = '\0';
            printf("get data from client %s: %s\n", inet_ntoa(addr_client.sin_addr), buf);
        }
    }
    
    close(fd);
    return 0;
}
