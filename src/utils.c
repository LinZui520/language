#include "syscall.h"

ssize_t str_len(const char *buf)
{
	int len = 0;
	while (buf[len] != '\0')
		len++;
	return len;
}

ssize_t str_cmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s2 != '\0') {
		if (*s1 != *s2)
			return 1;
		s1++;
		s2++;
	}
	if (*s1 == '\0' && *s2 == '\0')
		return 0;
	return 1;
}

void str_copy(char *dest, const char *src)
{
	while (*src != '\0') {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
}

ssize_t is_all_digit(char *buf)
{
	while (*buf != '\0') {
		if (*buf < '0' || *buf > '9')
			return 0;
		buf++;
	}
	return 1;
}

ssize_t alloc_memory(size_t size)
{
	ssize_t addr = brk((void *)0);
	brk((void *)(addr + size));
	return addr;
}
