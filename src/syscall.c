#include "syscall.h"

// 0号系统调用 读取文件
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

// 1号系统调用 写入文件
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

// 2号系统调用 打开文件
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

// 3号系统调用 关闭文件
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

// 8号系统调用 移动文件指针
ssize_t lseek(ssize_t fd, ssize_t offset, ssize_t whence)
{
	ssize_t result;
	asm volatile("movq $8, %%rax\n\t"
		     "movq %1, %%rdi\n\t"
		     "movq %2, %%rsi\n\t"
		     "movq %3, %%rdx\n\t"
		     "syscall\n\t"
		     "movq %%rax, %0\n\t"
		     : "=r"(result)
		     : "r"(fd), "r"(offset), "r"(whence)
		     : "rax", "rdi", "rsi", "rdx", "memory");
	return result;
}

// 12号系统调用 移动brk指针
ssize_t brk(void *addr)
{
	ssize_t result;
	asm volatile("movq $12, %%rax\n\t"
		     "movq %1, %%rdi\n\t"
		     "syscall\n\t"
		     "movq %%rax, %0\n\t"
		     : "=r"(result)
		     : "r"(addr)
		     : "rax", "rdi", "memory");
	return result;
}

// 59号系统调用 执行程序
int execve(const char *filename, char *const *argv, char *const *envp)
{
	int result;
	asm volatile("movq $59, %%rax\n\t"
		     "movq %1, %%rdi\n\t"
		     "movq %2, %%rsi\n\t"
		     "movq %3, %%rdx\n\t"
		     "syscall\n\t"
		     "movl %%eax, %0\n\t"
		     : "=r"(result)
		     : "r"(filename), "r"(argv), "r"(envp)
		     : "eax", "rdi", "rsi", "rdx", "memory");
	return result;
}
