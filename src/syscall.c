#include "syscall.h"

ssize_t read(ssize_t fd, char *buf, size_t count)
{
	ssize_t result;
	asm volatile("movq $0, %%rax\n\t"
		     "movq %1, %%rdi\n\t"
		     "movq %2, %%rsi\n\t"
		     "movq %3, %%rdx\n\t"
		     "syscall\n\t"
		     "movq %%rax, %0\n\t"
		     : "=r"(result)
		     : "r"(fd), "r"(buf), "r"(count)
		     : "rax", "rdi", "rsi", "rdx", "memory");
	return result;
}

ssize_t write(ssize_t fd, const char *buf, size_t count)
{
	ssize_t result;
	asm volatile("movq $1, %%rax\n\t"
		     "movq %1, %%rdi\n\t"
		     "movq %2, %%rsi\n\t"
		     "movq %3, %%rdx\n\t"
		     "syscall\n\t"
		     "movq %%rax, %0\n\t"
		     : "=r"(result)
		     : "r"(fd), "r"(buf), "r"(count)
		     : "rax", "rdi", "rsi", "rdx", "memory");
	return result;
}

ssize_t open(const char *pathname, int flags, int mode)
{
	ssize_t result;
	asm volatile("movq $2, %%rax\n\t"
		     "movq %1, %%rdi\n\t"
		     "movl %2, %%esi\n\t"
		     "movl %3, %%edx\n\t"
		     "syscall\n\t"
		     "movq %%rax, %0\n\t"
		     : "=r"(result)
		     : "r"(pathname), "r"(flags), "r"(mode)
		     : "rax", "rdi", "esi", "edx", "memory");
	return result;
}

ssize_t close(ssize_t fd)
{
	ssize_t result;
	asm volatile("movq $3, %%rax\n\t"
		     "movq %1, %%rdi\n\t"
		     "syscall\n\t"
		     "movq %%rax, %0\n\t"
		     : "=r"(result)
		     : "r"(fd)
		     : "rax", "rdi", "memory");
	return result;
}
