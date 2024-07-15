#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

enum AST_expr_type {
	AST_EXPR_ROOT, // 根节点
	AST_EXPR_IDENTIFIER, // 标识符 a main
	AST_EXPR_NUMBER, // 数字常量 1
	AST_EXPR_KEYWORD, // 关键字
	AST_EXPR_BINARY, // 二元操作符
	AST_EXPR_CALL, // 函数调用
	AST_EXPR_PROTOTYPE, // 函数原型
	AST_EXPR_BODY, // 函数体
	AST_EXPR_FUNCTION // 函数定义
};

struct AST_expr {
	enum AST_expr_type type;
	union {
		// 根节点
		struct {
			struct AST_expr **function;
			int count;
		} root;

		// 数字常量
		int number;

		// 标识符
		const char *identifier;

		// 关键字
		const char *keyword;

		// 二元操作符
		struct {
			const char *oparator;
			struct AST_expr *left;
			struct AST_expr *right;
		} binary;

		// 函数调用
		struct {
			struct AST_expr *name;
			struct AST_expr **args;
			int argc;
		} call;

		// 函数原型
		struct {
			struct AST_expr *name;
			struct AST_expr **args;
			int argc;
		} prototype;

		// 函数体
		struct {
			struct AST_expr **expr;
			int count;
		} body;

		// 函数定义
		struct {
			struct AST_expr *prototype;
			struct AST_expr *body;
		} function;
	} value;
};

void print_AST(struct AST_expr *expr, int deepin);

struct AST_expr *parser(struct token *tokens);

#endif
