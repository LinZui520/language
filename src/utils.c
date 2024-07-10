#include "syscall.h"

ssize_t strlen(const char *buf)
{
	int len = 0;
	while (buf[len] != '\0')
		len++;
	return len;
}
