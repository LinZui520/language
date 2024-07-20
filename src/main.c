#include "syscall.h"
#include "utils.h"
#include "lexer.h"
#include "io.h"
#include "parser.h"
#include "semantics.h"
#include "asm.h"

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
		print("打开文件失败\n");
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
		print("读取文件失败\n");
		return 1;
	}
	buf[length] = '\0';
	print("读取文件成功\n");

	// 词法分析
	struct token *tokens = lexer(buf);

	print("\n词法分析结果:\n");
	struct token *tmp = tokens;
	while (tmp != (void *)0) {
		print("%s:  %s    ", get_string_by_token_type(tmp->type),
		      tmp->value);
		print("<line: %d, column: %d>\n", tmp->line, tmp->column);
		tmp = tmp->next;
	}

	// 语法分析
	struct AST_expr *root = parser(tokens);

	print("\n语法分析结果:\n");
	print_AST(root, 1);

	// 语义分析
	struct global_symbol_table *table = semantic_analysis(root);

	if (table == (void *)0) {
		return 1;
	}

	// 生成汇编代码
	char *code = asm_code_generator(root, table);

	// 将汇编代码写入文件
	ssize_t asm_fd = open("main.s", 65, 511);
	write(asm_fd, code, str_len(code));

	// 关闭文件
	close(fd);
	close(asm_fd);

	const char *cmd_filename = "/usr/bin/gcc";
	char *const cmd_argv[] = { "/usr/bin/gcc",
				   "-nostdlib",
				   "-fno-stack-protector",
				   "main.s",
				   "-o",
				   "main",
				   0 };
	char *const cmd_envp[] = { "PATH=/usr/bin:/bin", 0 };

	// 程序控制权转交给gcc
	execve(cmd_filename, cmd_argv, cmd_envp);

	return 0;
}
