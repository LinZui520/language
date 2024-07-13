#ifndef UTILS_H
#define UTILS_H

#include "syscall.h"

// 计算字符串长度
ssize_t str_len(const char *buf);

// 比较两个字符串是否相等 相等返回0 不相等返回1
ssize_t str_cmp(const char *s1, const char *s2);

void str_copy(char *dest, const char *src);

ssize_t is_all_digit(const char *buf);

int atoi(const char *buf);

ssize_t alloc_memory(size_t size);

#endif
