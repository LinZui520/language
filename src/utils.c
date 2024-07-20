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

void str_cat(char *dest, const char *src)
{
	while (*dest != '\0')
		dest++;
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

ssize_t atoi(const char *buf)
{
	ssize_t num = 0;
	int flag = 1;
	if (*buf == '-') {
		buf++;
		flag = -1;
	}
	while (*buf != '\0') {
		num = num * 10 + *buf - '0';
		buf++;
	}
	return flag * num;
}

ssize_t alloc_memory(size_t size)
{
	ssize_t addr = brk((void *)0);
	brk((void *)(addr + size));
	return addr;
}

char *itoa(ssize_t num)
{
	char *buf = (char *)alloc_memory(sizeof(char) * 128);
	char *p = buf;
	if (num < 0) {
		*p = '-';
		p++;
		num = -num;
	}
	if (num == 0) {
		*p = '0';
		p++;
	}
	char *q = p;
	while (num) {
		*p = num % 10 + '0';
		p++;
		num /= 10;
	}
	*p = '\0';
	p--;
	while (q < p) {
		char tmp = *q;
		*q = *p;
		*p = tmp;
		q++;
		p--;
	}
	return buf;
}
