#ifndef LEXER_H
#define LEXER_H

#include "syscall.h"
#include "utils.h"

enum token_type {
	// 关键字
	// TOKEN_KEYWORD_VOID, //关键字 void
	TOKEN_KEYWORD_VAR, //关键字 var
	TOKEN_KEYWORD_FUNC, //关键字 func
	// TOKEN_KEYWORD_IF, //关键字 if
	// TOKEN_KEYWORD_ELSE, //关键字 else
	// TOKEN_KEYWORD_FOR, //关键字 for
	// TOKEN_KEYWORD_DO, //关键字 do
	// TOKEN_KEYWORD_WHILE, //关键字 while
	// TOKEN_KEYWORD_CONTINUE, //关键字 continue
	// TOKEN_KEYWORD_BREAK, //关键字 break
	TOKEN_KEYWORD_RETURN, //关键字 return

	// 标识符
	TOKEN_IDENTIFIER, // 标识符 如 变量名 函数名

	// 常量
	TOKEN_CONSTANT_INTEGER, // 整数常量

	// 标点符号
	TOKEN_PUNCTUATOR_ASSIGN, // =
	TOKEN_PUNCTUATOR_ADD, // +
	TOKEN_PUNCTUATOR_SUBTRACT, // -
	TOKEN_PUNCTUATOR_MULTIPLY, // *
	TOKEN_PUNCTUATOR_DIVIDE, // /
	// TOKEN_PUNCTUATOR_EQUAL, // ==
	// TOKEN_PUNCTUATOR_NOT_EQUAL, // !=
	// TOKEN_PUNCTUATOR_LESS, // <
	// TOKEN_PUNCTUATOR_LESS_EQUAL, // <=
	// TOKEN_PUNCTUATOR_GREATER, // >
	// TOKEN_PUNCTUATOR_GREATER_EQUAL, // >=
	// TOKEN_PUNCTUATOR_AND, // &&
	// TOKEN_PUNCTUATOR_OR, // ||
	// TOKEN_PUNCTUATOR_NOT, // !
	TOKEN_PUNCTUATOR_SEMICOLON, // ;
	TOKEN_PUNCTUATOR_COMMA, // ,
	TOKEN_PUNCTUATOR_LEFT_PARENTHESIS, // (
	TOKEN_PUNCTUATOR_RIGHT_PARENTHESIS, // )
	TOKEN_PUNCTUATOR_LEFT_BRACKET, // {
	TOKEN_PUNCTUATOR_RIGHT_BRACKET, // }
	// TOKEN_PUNCTUATOR_LEFT_SQUARE_BRACKET, // [
	// TOKEN_PUNCTUATOR_RIGHT_SQUARE_BRACKET, // ]

	TOKEN_EOF // 文件结束符
};

struct token {
	enum token_type type;
	const char *value;
	ssize_t line;
	ssize_t column;
	struct token *next;
};

char *get_string_by_token_type(enum token_type type);

struct token *lexer(const char *code);

#endif
