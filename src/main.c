#include "syscall.h"
#include "utils.h"
#include "lexer.h"
#include "io.h"
#include "parser.h"

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
	// root
	print("%s\n", get_string_by_expr_type(root->type));
	for (int i = 0; i < root->value.root.count; i++) {
		struct AST_expr *func = root->value.root.function[i];
		// function
		print("|---%s\n", get_string_by_expr_type(func->type));
		// prototype
		print("|------%s\n",
		      get_string_by_expr_type(
			      func->value.function.prototype->type));
		// prototype name
		print("|---------func_name: %s:%s\n",
		      get_string_by_expr_type(
			      func->value.function.prototype->value.prototype
				      .name->type),
		      func->value.function.prototype->value.prototype.name
			      ->value.identifier);
		// prototype args
		for (int j = 0;
		     j < func->value.function.prototype->value.prototype.argc;
		     j++) {
			print("|---------func_args: %s:%s\n",
			      get_string_by_expr_type(
				      func->value.function.prototype->value
					      .prototype.args[j]
					      ->type),
			      func->value.function.prototype->value.prototype
				      .args[j]
				      ->value.identifier);
		}

		// body
		print("|------%s\n",
		      get_string_by_expr_type(func->value.function.body->type));
		for (int j = 0; j < func->value.function.body->value.body.count;
		     j++) {
			// body expr
			// todo:
			print("|---------%s\n", "body expr");
		}
	}

	// 关闭文件
	close(fd);

	return 0;
}
