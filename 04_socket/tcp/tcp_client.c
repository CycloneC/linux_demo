#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define PORT_SEVER      8888
#define IP_SEVER        "127.0.0.1"

int main(int argc, char const *argv[])
{
    int ret, fd;
    struct sockaddr_in addr_sever; //struct sockaddr不用，而用struct sockaddr_in，比较好对元素赋值
    char buf[1024] = {0};

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
        printf("socket err\n");
        return -1;
    }

    memset(&addr_sever, 0, sizeof(addr_sever));
    addr_sever.sin_family      = AF_INET;
    addr_sever.sin_port        = htons(PORT_SEVER);
    if (0 == inet_aton(IP_SEVER, &addr_sever.sin_addr)) //地址转换
    {
        printf("invalid server_ip\n");
        return -1;
    }

    ret = connect(fd, (struct sockaddr *)&addr_sever, sizeof(struct sockaddr));
    if (-1 == ret)
    {
        printf("connect err\n");
        return -1;
    }

    while (1)
    {
        if (fgets(buf, 1024, stdin))
        {
            ret = send(fd, buf, strlen(buf), 0);
            if (ret <= 0)
            {
                close(fd);
                return -1;
            }
        }
    }

    return 0;
}
