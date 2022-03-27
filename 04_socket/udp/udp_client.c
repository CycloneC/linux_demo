#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define USE_SEND        1 //是否使用send函数，udp使用send函数需要先调用client

#define PORT_SEVER      8888
#define IP_SEVER        "127.0.0.1"

int main(int argc, char const *argv[])
{
    int ret, fd;
    struct sockaddr_in addr_sever; //struct sockaddr不用，而用struct sockaddr_in，比较好对元素赋值
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
    if (0 == inet_aton(IP_SEVER, &addr_sever.sin_addr)) //地址转换
    {
        printf("invalid server_ip\n");
        return -1;
    }

#if USE_SEND
	ret = connect(fd, (const struct sockaddr *)&addr_sever, sizeof(struct sockaddr));	
	if (-1 == ret)
	{
		printf("connect error!\n");
		return -1;
	}
#endif

    while (1)
    {
        if (fgets(buf, 1024, stdin))
        {
        #if USE_SEND
            ret = send(fd, buf, strlen(buf), 0);
            if (ret <= 0)
            {
                close(fd);
                return -1;
            }
        #else
            ret = sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&addr_sever, sizeof(struct sockaddr));
            if (ret <= 0)
            {
                close(fd);
                return -1;
            }
        #endif
        }
    }
    
    return 0;
}