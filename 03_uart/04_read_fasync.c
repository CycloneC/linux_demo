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
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

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
    
    newtio.c_cc[VMIN]  = 0;  /* ??????????????????????????????: ??????????????????????????????! */
    newtio.c_cc[VTIME] = 0;  /* ?????????1??????????????????: 
                              * ??????VMIN??????10??????????????????10??????????????????,
                              * ???????????????????????????????????????? ????????????VTIME(?????????100ms)
                              * ??????VTIME=1?????????: 
                              *    100ms???????????????????????????????????????
                              *    ??????100ms??????????????????1?????????????????????????????????????????????VMIN??????????????????
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

    if (fcntl(fd, F_SETFL, 0) < 0) /*???????????????????????????*/
    {
        printf("fcntl failed");
        return -1;
    }

    return fd;
}

int fd = -1;

/**
 * @brief ??????????????????
 * 
 * @param sig 
 */
void my_sig_handler(int sig)
{
    int ret = -1;
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
                return;
            }
            break;
        }
        else if (ret < 0)
        {
            printf("read err");
            return;
        }

        read_len += ret;
    }
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
    int flags = -1;

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

    /* ???????????????????????? */
    signal(SIGIO, my_sig_handler);

    /* ???APP?????????????????????????????? */
	fcntl(fd, F_SETOWN, getpid());

    /* ??????"????????????" */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | FASYNC);

    while (1)
    {
        printf("main loop\n");
        sleep(2);
    }

    return 0;
}
