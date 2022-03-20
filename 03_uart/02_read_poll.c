/**
 * @file 02_read_poll.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-03-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

/**
 * @brief Set the opt object
 *        set_opt(fd, 115200, 8, 'N', 1)
 * @param fd 
 * @param nSpeed 
 * @param nBits 
 * @param nEvent 
 * @param nStop 
 * @return int 
 */
int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    
    if (tcgetattr(fd,&oldtio) != 0)
    { 
        perror("SetupSerial 1");
        return -1;
    }
    
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD; 
    newtio.c_cflag &= ~CSIZE; 

    newtio.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    newtio.c_oflag  &= ~OPOST;   /*Output*/

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
    break;
    case 8:
        newtio.c_cflag |= CS8;
    break;
    }

    switch( nEvent )
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
    break;
    case 'E': 
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
    break;
    case 'N': 
        newtio.c_cflag &= ~PARENB;
    break;
    }

    switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
    break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
    break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
    break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
    break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
    break;
    }
    
    if( nStop == 1 )
        newtio.c_cflag &= ~CSTOPB;
    else if ( nStop == 2 )
        newtio.c_cflag |= CSTOPB;
    
    newtio.c_cc[VMIN]  = 0;  /* 读数据时的最小字节数: 没读到这些数据不返回! */
    newtio.c_cc[VTIME] = 0;  /* 等待第1个数据的时间: 
                              * 比如VMIN设为10表示至少读到10个数据才返回,
                              * 但是没有数据总不能一直等吧? 可以设置VTIME(单位是100ms)
                              * 假设VTIME=1，表示: 
                              *    100ms内一个数据都没有的话就返回
                              *    如果100ms内至少读到了1个字节，那就继续等待，完全读到VMIN个数据再返回
                              */

    tcflush(fd,TCIFLUSH);
    
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }

    return 0;
}

/**
 * @brief
 * 
 * @param port 
 * @return int 
 */
int open_port(const char *port)
{
    int fd = -1;

    fd = open(port, O_RDWR|O_NOCTTY|O_NDELAY);
    if (fd < 0)
    {
        printf("can not open %s, err fd=%d\n", port, fd);
        return -1;
    }

    if (fcntl(fd, F_SETFL, 0) < 0) /*设置串口为阻塞状态*/
    {
        printf("fcntl failed");
        return -1;
    }

    return fd;
}

/**
 * @brief 
 * 
 * @param fd 
 * @return int 
 */
int read_data(int fd)
{
    int ret = -1;
    int nfds = 0;
    struct pollfd fds[1]; //可以监听多个

    fds[0].fd = fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    nfds = 1;

    while (1)
    {
        ret = poll(fds, nfds, 5000);
        if (ret == 0)
        {
            printf("time out\n");
            continue;
        }
        else if (ret < 0)
        {
            printf("poll err, ret=%d\n", ret);
            break;
        }

        for (int i = 0; i < nfds; i++)
        {
            if (fds[i].revents == 0)
            {
                continue;
            }

            if (fds[i].revents != fds[i].events)
            {
                printf("revents err, revents=%d events=%d\n", fds[i].revents, fds[i].events);
                return -1;
            }

            int read_len = 0;
            char buf[1024] = {0};
            while (1)
            {
                ret = read(fd, buf + read_len, 1024);
                if (ret == 0)
                {
                    printf("read len=%d, buf=%s\n", read_len, buf);
                    if (write(fd, buf, read_len) != read_len)
                    {
                        printf("write err");
                        return -1;
                    }
                    break;
                }
                else if (ret < 0)
                {
                    printf("read err");
                    return -1;
                }

                read_len += ret;
            }
        }
    }

    return -1;
}

/**
 * @brief ./test /dev/ttyUSB0
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char const *argv[])
{
    int fd = -1;

    if (argc < 2)
    {
        printf("Usage: %s <uart>", argv[0]);
        return -1;
    }

    fd = open_port(argv[1]);
    if (fd < 0)
    {
        printf("open port failed");
        return -1;
    }
    
    if (set_opt(fd, 115200, 8, 'N', 1) < 0)
    {
        printf("set opt failed");
        return -1;
    }

    if (read_data(fd) < 0)
    {
        printf("read data failed");
        return -1;
    }

    return 0;
}
