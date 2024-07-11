#include "syscall.h"
#include "utils.h"
#include "lexer.h"
#include "io.h"

int main(int argc, char *argv[])
{
	// 判断输入参数是否正确
	if (argc != 2) {
		print("参数错误\n");
		return 1;
	}

	// 打开文件 获取文件描述符
	ssize_t fd = open(argv[1], 0, 0);
	if (fd < 0) {
		print("打开文件失败");
		return 1;
	}
	print("打开文件成功\n");

	// 获取文件长度
	ssize_t length = lseek(fd, 0, SEEK_END);

	// 重置文件指针
	lseek(fd, 0, SEEK_SET);

	// 读取文件内容
	char buf[length + 1];
	ssize_t count = read(fd, buf, length);
	if (count < 0) {
		print("读取文件失败");
		return 1;
	}
	buf[length] = '\0';
	print("读取文件成功\n");

	// 词法分析
	struct token *head = lexer(buf);
	while (head != (void *)0) {
		print("%s:  %s    ", get_string_by_token_type(head->type),
		      head->value);
		print("<line: %d, column: %d>\n\n", head->line, head->column);
		head = head->next;
	}

	// 关闭文件
	close(fd);

	return 0;
}
