#include <stdio.h>
#include "sub.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("hello world\n");
		return 0;
	}

    sub();

	printf("hello %s\n", argv[1]);

	return 0;
}
