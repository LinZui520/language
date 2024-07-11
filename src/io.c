#include "syscall.h"
#include "utils.h"

void scan(const char *format, ...)
{
	__builtin_va_list args;
	__builtin_va_start(args, format);

	while (*format != '\0') {
		if (*format != '%') {
			format++;
			continue;
		}

		format++;
		if (*format == 's') {
			char *str = __builtin_va_arg(args, char *);
			char buf[4096];
			while (*buf != '\n') {
				read(STDIN, buf, 1);
				*str++ = *buf;
			}
			*(--str) = '\0';
		} else if (*format == 'd') {
			int *num = __builtin_va_arg(args, int *);
			char buf[16];
			int i = 0;
			int temp = 0;
			while (1) {
				read(STDIN, &buf[i++], 1);
				if (buf[i - 1] == '-' && i == 1)
					continue;
				if (buf[i - 1] < '0' || buf[i - 1] > '9')
					break;
				temp = temp * 10 + buf[i - 1] - '0';
			}
			*num = buf[0] == '-' ? -temp : temp;
		} else if (*format == 'c') {
			char *c = __builtin_va_arg(args, char *);
			read(STDIN, c, 1);
		}
		format++;
	}

	__builtin_va_end(args);
}

void print(const char *format, ...)
{
	__builtin_va_list args;
	__builtin_va_start(args, format);

	while (*format != '\0') {
		if (*format != '%') {
			write(STDOUT, format++, 1);
			continue;
		}

		format++;
		if (*format == '%') {
			write(STDOUT, format, 1);
		} else if (*format == 's') {
			char *str = __builtin_va_arg(args, char *);
			write(STDOUT, str, strlen(str));
		} else if (*format == 'd') {
			int num = __builtin_va_arg(args, int);
			int temp = num < 0 ? -num : num;
			char buffer[32];
			int i = 0;
			while (temp != 0) {
				buffer[i++] = temp % 10 + '0';
				temp /= 10;
			}

			if (num == 0)
				buffer[i++] = '0';
			if (num < 0)
				buffer[i++] = '-';
			buffer[i] = '\0';

			int left = 0;
			int right = i - 1;
			while (left < right) {
				buffer[left] ^= buffer[right];
				buffer[right] ^= buffer[left];
				buffer[left++] ^= buffer[right--];
			}

			write(STDOUT, buffer, i);
		} else if (*format == 'c') {
			char c = __builtin_va_arg(args, int);
			write(STDOUT, &c, 1);
		}
		format++;
	}

	__builtin_va_end(args);
}
