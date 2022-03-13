#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int len = -1;
    int fd = -1;
    int fd_new = -1;
    char buf[1024] = {0};

    if (argc != 3)
    {
        printf("Usage: %s [file] [file_new]\n", argv[0]);
        return -1;
    }

    /*打开文件*/
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        printf("can not open %s, err fd=%d\n", argv[1], fd);
        return -1;
    }
    
    /*创建新文件*/
    fd_new = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd_new < 0)
    {
        printf("can not open %s, err fd=%d\n", argv[2], fd_new);
        return -1;
    }

    /*循环读写文件*/
    while((len = read(fd, buf, 1024)) > 0)
    {
        if (write(fd_new, buf, len) != len)
        {
            printf("can not write %s, write_len=%d\n", argv[2], len);
            return -1;
        }
    }
    
    /*关闭文件*/
    close(fd);
    close(fd_new);

    return 0;
}
