#include "syscall.h"

// 打印字符串
#define log(x) write(STDOUT, x, strlen(x))

// 计算字符串长度
ssize_t strlen(const char *buf);

ssize_t alloc_memory(size_t size);
