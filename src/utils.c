#include "syscall.h"

ssize_t strlen(const char *buf)
{
	int len = 0;
	while (buf[len] != '\0')
		len++;
	return len;
}

ssize_t alloc_memory(size_t size)
{
	ssize_t addr = brk((void *)0);
	brk((void *)(addr + size));
	return addr;
}
