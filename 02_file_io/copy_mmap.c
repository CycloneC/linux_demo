#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>

int main(int argc, char const *argv[])
{
    int fd = -1;
    int fd_new = -1;
    char *buf = NULL;
    struct stat stat;

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

    /*获取文件大小*/
    if (fstat(fd, &stat) < 0)
    {
        printf("can not get fstat of file %s\n", argv[1]);
        return -1;
    }

    /*映射文件*/
    buf = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED)
    {
        printf("can not mmap file %s\n", argv[1]);
		return -1;
    }

    /*创建新文件*/
    fd_new = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd_new < 0)
    {
        printf("can not open %s, err fd=%d\n", argv[2], fd_new);
        return -1;
    }

    /*写文件*/
    if (write(fd_new, buf, stat.st_size) != stat.st_size)
    {
        printf("can not write %s, write_len=%ld\n", argv[2], stat.st_size);
        return -1;
    }

    /*关闭文件*/
    close(fd);
    close(fd_new);

    return 0;
}
