#include "parser.h"
#include "lexer.h"
#include "utils.h"

char *get_string_by_expr_type(enum AST_expr_type type)
{
	char *str = (char *)alloc_memory(sizeof(char) * 32);
	switch (type) {
	case AST_EXPR_ROOT:
		str_copy(str, "root");
		break;
	case AST_EXPR_IDENTIFIER:
		str_copy(str, "identifier");
		break;
	case AST_EXPR_NUMBER:
		str_copy(str, "number");
		break;
	case AST_EXPR_UNARY:
		str_copy(str, "unary");
		break;
	case AST_EXPR_CALL:
		str_copy(str, "call");
		break;
	case AST_EXPR_PROTOTYPE:
		str_copy(str, "prototype");
		break;
	case AST_EXPR_BODY:
		str_copy(str, "body");
		break;
	case AST_EXPR_FUNCTION:
		str_copy(str, "function");
		break;
	default:
		str_copy(str, "unknown");
		break;
	}
	return str;
}
/*
 * 假如栈里面的数据为 a = 1 + 2 * 3 这7个节点
 * 那么这个函数会将其解析为
 *                 =
 *               /   \
 *              a     +
 *                  /   \
 *                 1     *
 *                     /   \
 *                    2     3
 * 并返回 = 这个 AST_expr 节点
 * 
 * body 节点的表达式解析是依靠这个函数实现的
 * 最终重复调用这个函数 函数体返回的节点大概长这样
 * 	         body
 *             /  |   \
 *            =  省略  省略
 *          /   \
 *         a     3
 * 省略的部分就是这个函数解析的返回的根节点
 */
struct AST_expr *stack_parsing_to_tree(struct AST_expr **stack, int count)
{
	// todo:
	return (void *)0;
}

/*
 * 这个函数主要是获取函数体表达式的数量
 * 事先分配好内存存储AST_expr节点
 * 解析表达式还是得依靠上面的 stack_parsing_to_tree 函数
 */
struct AST_expr *create_body_expr(struct token *tokens)
{
	struct AST_expr *body =
		(struct AST_expr *)alloc_memory(sizeof(struct AST_expr));
	body->type = AST_EXPR_BODY;

	int count = 0;
	while (tokens->type != TOKEN_PUNCTUATOR_RIGHT_BRACKET) {
		if (tokens->type == TOKEN_PUNCTUATOR_SEMICOLON) {
			count++;
		}
		tokens = tokens->next;
	}

	body->value.body.expr = (struct AST_expr **)alloc_memory(
		sizeof(struct AST_expr *) * count);
	body->value.body.count = count;

	for (int i = 0; i < count; i++)
		body->value.body.expr[i] = (struct AST_expr *)alloc_memory(
			sizeof(struct AST_expr));

	return body;
}

#include "io.h"
/*
 * 这个函数主要是获取函数原型
 * 也就是获取 函数名称 和 需要的参数名称和数量 
 * 且会将tokens链表指向函数开头的 { 节点
 * 比如 func add(var a, var b) 
 * {  <===tokens会指向这个元素
 * 
 * }
 * 函数返回 prototype节点 还是上面这个函数的例子 这棵树的样子大概长这样
 *                 prototype
 *                /    |    \
 *               add   a     b  
 */
struct AST_expr *create_prototype_expr(struct token **tokens)
{
	// 跳过 func 关键字，指向函数名称 token
	*tokens = (*tokens)->next;

	struct AST_expr *prototype =
		(struct AST_expr *)alloc_memory(sizeof(struct AST_expr));
	prototype->type = AST_EXPR_PROTOTYPE;
	prototype->value.prototype.name =
		(struct AST_expr *)alloc_memory(sizeof(struct AST_expr));
	prototype->value.prototype.name->type = AST_EXPR_IDENTIFIER;
	prototype->value.prototype.name->value.identifier = (*tokens)->value;

	// 跳过函数名称，指向左括号
	*tokens = (*tokens)->next;

	// 获取参数数量
	int count = 0;
	struct token *temp = *tokens;
	while (temp->type != TOKEN_PUNCTUATOR_RIGHT_PARENTHESIS) {
		if (temp->type == TOKEN_IDENTIFIER) {
			count++;
		}
		temp = temp->next;
	}

	prototype->value.prototype.args = (struct AST_expr **)alloc_memory(
		sizeof(struct AST_expr *) * count);
	prototype->value.prototype.argc = count;

	int index = 0;
	while ((*tokens)->type != TOKEN_PUNCTUATOR_RIGHT_PARENTHESIS) {
		if ((*tokens)->type == TOKEN_IDENTIFIER) {
			prototype->value.prototype.args[index] =
				(struct AST_expr *)alloc_memory(
					sizeof(struct AST_expr));
			prototype->value.prototype.args[index]->type =
				AST_EXPR_IDENTIFIER;
			prototype->value.prototype.args[index]
				->value.identifier = (*tokens)->value;
			index++;
		}
		*tokens = (*tokens)->next;
	}

	return prototype;
}

/*
 * 这个函数主要是创建好 root 节点和获取代码中函数的数量
 * 以便事先分配好内存存储 AST_EXPR_FUNCTION 节点
 * 返回的节点这颗树大概长这样
 *                         root
 *                      /        \
 *                  func          func
 *                /      \      /      \
 *       prototype      body  prototype  body
 */
struct AST_expr *create_root_expr(struct token *tokens)
{
	int count = 0;
	while (tokens != (void *)0) {
		if (tokens->type == TOKEN_KEYWORD_FUNC) {
			count++;
		}
		tokens = tokens->next;
	}

	struct AST_expr *root =
		(struct AST_expr *)alloc_memory(sizeof(struct AST_expr));
	root->type = AST_EXPR_ROOT;
	root->value.root.function = (struct AST_expr **)alloc_memory(
		sizeof(struct AST_expr *) * count);
	root->value.root.count = count;
	for (int i = 0; i < count; i++) {
		root->value.root.function[i] = (struct AST_expr *)alloc_memory(
			sizeof(struct AST_expr));
		root->value.root.function[i]->type = AST_EXPR_FUNCTION;
	}
	return root;
}

/*
 * 这个函数是整个解析的入口
 * 会调用上面的函数来解析表达式
 * 最终将 tokens 链表解析为 AST_expr 节点
 * 返回的节点大概长这样
 * 		                root
 * 	                    /          \
 *                    function         function
 *                   /        \      /           \
 *             prototype    body   prototype      body
 *               /         / | \   /   |   \       |
 *             main       *  *  * add  a    b      + (return)
 *                                                / \
 *                                               a   b
 */
struct AST_expr *parser(struct token *tokens)
{
	struct AST_expr *root = create_root_expr(tokens);
	int current_function_index = 0;
	int current_body_index = 0;

	// 将表达式节点存储到栈中 用于后续的表达式解析
	struct AST_expr **stack = (struct AST_expr **)alloc_memory(
		sizeof(struct AST_expr *) * 32);
	int stack_index = 0;

	while (tokens != (void *)0) {
		if (tokens->type == TOKEN_KEYWORD_FUNC) {
			root->value.root.function[current_function_index]
				->value.function.prototype =
				create_prototype_expr(&tokens);
			root->value.root.function[current_function_index]
				->value.function.body =
				create_body_expr(tokens);
		} else if (tokens->type == TOKEN_KEYWORD_VAR) {
			// 关键字 var
		} else if (tokens->type == TOKEN_KEYWORD_RETURN) {
			// 关键字 return
		} else if (tokens->type == TOKEN_IDENTIFIER) {
			// 标识符 a main
			stack[stack_index] = (struct AST_expr *)alloc_memory(
				sizeof(struct AST_expr));
			stack[stack_index]->type = AST_EXPR_IDENTIFIER;
			stack[stack_index]->value.identifier = tokens->value;
			stack_index++;
		} else if (tokens->type == TOKEN_CONSTANT_INTEGER) {
			// 常量 1 2
			stack[stack_index] = (struct AST_expr *)alloc_memory(
				sizeof(struct AST_expr));
			stack[stack_index]->type = AST_EXPR_NUMBER;
			stack[stack_index]->value.number = atoi(tokens->value);
			stack_index++;
		} else if (tokens->type == TOKEN_PUNCTUATOR_ASSIGN ||
			   tokens->type == TOKEN_PUNCTUATOR_ADD ||
			   tokens->type == TOKEN_PUNCTUATOR_SUBTRACT ||
			   tokens->type == TOKEN_PUNCTUATOR_MULTIPLY ||
			   tokens->type == TOKEN_PUNCTUATOR_DIVIDE) {
			// = + - * /
			stack[stack_index] = (struct AST_expr *)alloc_memory(
				sizeof(struct AST_expr));
			stack[stack_index]->type = AST_EXPR_UNARY;
			stack[stack_index]->value.unary.oparator =
				tokens->value;
			stack_index++;
		} else if (tokens->type == TOKEN_PUNCTUATOR_SEMICOLON) {
			// ;
			root->value.root.function[current_function_index]
				->value.function.body->value.body
				.expr[current_body_index] =
				stack_parsing_to_tree(stack, stack_index);
			current_body_index++;
		} else if (tokens->type == TOKEN_PUNCTUATOR_COMMA) {
			// ,
		} else if (tokens->type == TOKEN_PUNCTUATOR_LEFT_PARENTHESIS) {
			// (
		} else if (tokens->type == TOKEN_PUNCTUATOR_RIGHT_PARENTHESIS) {
			// )
		} else if (tokens->type == TOKEN_PUNCTUATOR_LEFT_BRACKET) {
			// {
		} else if (tokens->type == TOKEN_PUNCTUATOR_RIGHT_BRACKET) {
			// }
			current_function_index++;
			current_body_index = 0;
		} else if (tokens->type == TOKEN_EOF) {
			break;
		}
		tokens = tokens->next;
	}

	return root;
}
