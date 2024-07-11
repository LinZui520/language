#include "lexer.h"
#include "syscall.h"
#include "utils.h"
#include "io.h"

char *get_string_by_token_type(enum token_type type)
{
	char *str = (char *)alloc_memory(sizeof(char) * 32);
	switch (type) {
	case TOKEN_KEYWORD_INT:
		str_copy(str, "token_keyword_int");
		break;
	case TOKEN_KEYWORD_RETURN:
		str_copy(str, "token_keyword_return");
		break;
	case TOKEN_PUNCTUATOR_ASSIGN:
		str_copy(str, "token_punctuator_assign");
		break;
	case TOKEN_PUNCTUATOR_ADD:
		str_copy(str, "token_punctuator_add");
		break;
	case TOKEN_PUNCTUATOR_SUBTRACT:
		str_copy(str, "token_punctuator_subtract");
		break;
	case TOKEN_PUNCTUATOR_MULTIPLY:
		str_copy(str, "token_punctuator_multiply");
		break;
	case TOKEN_PUNCTUATOR_DIVIDE:
		str_copy(str, "token_punctuator_divide");
		break;
	case TOKEN_PUNCTUATOR_SEMICOLON:
		str_copy(str, "token_punctuator_semicolon");
		break;
	case TOKEN_PUNCTUATOR_COMMA:
		str_copy(str, "token_punctuator_comma");
		break;
	case TOKEN_PUNCTUATOR_LEFT_PARENTHESIS:
		str_copy(str, "token_punctuator_left_parenthesis");
		break;
	case TOKEN_PUNCTUATOR_RIGHT_PARENTHESIS:
		str_copy(str, "token_punctuator_right_parenthesis");
		break;
	case TOKEN_PUNCTUATOR_LEFT_BRACKET:
		str_copy(str, "token_punctuator_left_bracket");
		break;
	case TOKEN_PUNCTUATOR_RIGHT_BRACKET:
		str_copy(str, "token_punctuator_right_bracket");
		break;
	case TOKEN_CONSTANT_INTEGER:
		str_copy(str, "token_constant_integer");
		break;
	case TOKEN_IDENTIFIER:
		str_copy(str, "token_identifier");
		break;
	case TOKEN_EOF:
		str_copy(str, "token_eof");
		break;
	default:
		str_copy(str, "unknown");
	}
	return str;
}

enum token_type get_token_type_by_string(const char *str)
{
	if (str_cmp(str, "int") == 0)
		return TOKEN_KEYWORD_INT;
	if (str_cmp(str, "return") == 0)
		return TOKEN_KEYWORD_RETURN;
	if (str_cmp(str, "=") == 0)
		return TOKEN_PUNCTUATOR_ASSIGN;
	if (str_cmp(str, "+") == 0)
		return TOKEN_PUNCTUATOR_ADD;
	if (str_cmp(str, "-") == 0)
		return TOKEN_PUNCTUATOR_SUBTRACT;
	if (str_cmp(str, "*") == 0)
		return TOKEN_PUNCTUATOR_MULTIPLY;
	if (str_cmp(str, "/") == 0)
		return TOKEN_PUNCTUATOR_DIVIDE;
	if (str_cmp(str, ";") == 0)
		return TOKEN_PUNCTUATOR_SEMICOLON;
	if (str_cmp(str, ",") == 0)
		return TOKEN_PUNCTUATOR_COMMA;
	if (str_cmp(str, "(") == 0)
		return TOKEN_PUNCTUATOR_LEFT_PARENTHESIS;
	if (str_cmp(str, ")") == 0)
		return TOKEN_PUNCTUATOR_RIGHT_PARENTHESIS;
	if (str_cmp(str, "{") == 0)
		return TOKEN_PUNCTUATOR_LEFT_BRACKET;
	if (str_cmp(str, "}") == 0)
		return TOKEN_PUNCTUATOR_RIGHT_BRACKET;
	if (is_all_digit(str))
		return TOKEN_CONSTANT_INTEGER;
	return TOKEN_IDENTIFIER;
}

struct token *new_token(enum token_type type, const char *value, ssize_t line,
			ssize_t column)
{
	struct token *token =
		(struct token *)alloc_memory(sizeof(struct token));
	token->type = type;
	token->value = value;
	token->line = line;
	token->column = column;
	token->next = (struct token *)0;
	return token;
}

// 将buf中的字符串转换为token并存储到链表尾端
void new_token_by_buf(char *buf, ssize_t *i, struct token **tail, ssize_t line,
		      ssize_t *column)
{
	char *str = (char *)alloc_memory(sizeof(char) * (*i));
	str_copy(str, buf);
	struct token *token =
		new_token(get_token_type_by_string(buf), str, line, *column);
	(*tail)->next = token;
	(*tail) = (*tail)->next;
        *column += *i - 1 ;
	buf[0] = '\0';
	*i = 0;
}

struct token *lexer(const char *code)
{
	ssize_t index = 0; // code的索引
	ssize_t line = 1; // 遍历code的行号
	ssize_t column = 1; // 列号code的列号

	// 存储当前token的字符串
	char buf[128];
	ssize_t i = 0;

	struct token *head = (struct token *)alloc_memory(sizeof(struct token));
	struct token *tail = head;
	while (code[index] != '\0') {
		if (code[index] == ' ') {
			if (buf[0] != '\0') {
				buf[i++] = '\0';
				new_token_by_buf(buf, &i, &tail, line, &column);
			}
			column++;
			index++;
			continue;
		} else if (code[index] == '\n') {
			if (buf[0] != '\0') {
				buf[i++] = '\0';
				new_token_by_buf(buf, &i, &tail, line, &column);
			}
			line++;
			column = 1;
			index++;
			continue;
		} else if (code[index] == '\t') {
			if (buf[0] != '\0') {
				buf[i++] = '\0';
				new_token_by_buf(buf, &i, &tail, line, &column);
			}
			column += 8;
			index++;
			continue;
		} else if (code[index] == '=' || code[index] == '+' ||
			   code[index] == '-' || code[index] == '*' ||
			   code[index] == '/' || code[index] == ';' ||
			   code[index] == ',' || code[index] == '(' ||
			   code[index] == ')' || code[index] == '{' ||
			   code[index] == '}') {
			if (buf[0] != '\0') {
				buf[i++] = '\0';
				new_token_by_buf(buf, &i, &tail, line, &column);
			}

			char *str = (char *)alloc_memory(sizeof(char) * 2);
			str[0] = code[index];
			str[1] = '\0';
			struct token *token =
				new_token(get_token_type_by_string(str), str,
					  line, column);
			tail->next = token;
			tail = tail->next;

			column++;
			index++;
		} else {
			buf[i++] = code[index++];
		}
	}
	struct token *token = new_token(TOKEN_EOF, "EOF", line, column);
	tail->next = token;
	tail = tail->next;

	return head->next;
}
