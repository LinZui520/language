#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "io.h"

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
	case AST_EXPR_KEYWORD:
		str_copy(str, "keyword");
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

void print_deepin(int deepin)
{
	switch (deepin) {
	case 1:
		print("|---------");
		break;
	case 2:
		print("|------------");
		break;
	case 3:
		print("|----------------");
		break;
	case 4:
		print("|-------------------");
		break;
	case 5:
		print("|----------------------");
		break;
	case 6:
		print("|-------------------------");
		break;
	}
}

void print_AST(struct AST_expr *expr, int deepin)
{
	if (expr == (void *)0)
		return;
	if (expr->type == AST_EXPR_ROOT) {
		print("%s\n", get_string_by_expr_type(expr->type));
		for (int i = 0; i < expr->value.root.count; i++) {
			print_AST(expr->value.root.function[i], deepin);
			print("|\n");
		}
	} else if (expr->type == AST_EXPR_FUNCTION) {
		print("|---%s\n", get_string_by_expr_type(expr->type));
		print_AST(expr->value.function.prototype, deepin);
		print_AST(expr->value.function.body, deepin);
	} else if (expr->type == AST_EXPR_PROTOTYPE) {
		print("|------%s\n", get_string_by_expr_type(expr->type));
		// prototype name
		print("|---------func_name: %s:%s\n",
		      get_string_by_expr_type(expr->value.prototype.name->type),
		      expr->value.prototype.name->value.identifier);
		// prototype args
		for (int i = 0; i < expr->value.prototype.argc; i++) {
			print("|---------func_args: %s:%s\n",
			      get_string_by_expr_type(
				      expr->value.prototype.args[i]->type),
			      expr->value.prototype.args[i]->value.identifier);
		}
	} else if (expr->type == AST_EXPR_BODY) {
		print("|------%s\n", get_string_by_expr_type(expr->type));
		for (int i = 0; i < expr->value.body.count; i++) {
			// body expr
			print_AST(expr->value.body.expr[i], deepin);
			print("|\n");
		}
	} else if (expr->type == AST_EXPR_IDENTIFIER) {
		print_deepin(deepin);
		print("%s\n", expr->value.identifier);
	} else if (expr->type == AST_EXPR_NUMBER) {
		print_deepin(deepin);
		print("%d\n", expr->value.number);
	} else if (expr->type == AST_EXPR_CALL) {
		print_deepin(deepin);
		print("call\n");
		print_AST(expr->value.call.name, deepin + 1);
		for (int i = 0; i < expr->value.call.argc; i++) {
			print_AST(expr->value.call.args[i], deepin + 1);
		}
	} else if (expr->type == AST_EXPR_UNARY) {
		print_AST(expr->value.unary.left, deepin + 1);
		print_deepin(deepin);
		print("-/\n");

		print_deepin(deepin);
		print("%s\n", expr->value.unary.oparator);

		print_deepin(deepin);
		print("-\\\n");
		print_AST(expr->value.unary.right, deepin + 1);
	}
}

// 获取操作符的优先级
int get_punctuator_priority(const char *oparator)
{
	if (str_cmp(oparator, "=") == 0)
		return 1;
	if (str_cmp(oparator, "+") == 0 || str_cmp(oparator, "-") == 0)
		return 2;
	if (str_cmp(oparator, "*") == 0 || str_cmp(oparator, "/") == 0)
		return 3;
	return 0;
}

/*
 * 假如数组里面的数据为 a = 1 + 2 * 3 这7个节点
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
struct AST_expr *array_parsing_to_tree(struct AST_expr **array, int count)
{
	int index = 0;
	struct AST_expr **stack = array;

	int flag = 0;
	for (int i = 0; i < count; i++) {
		if (array[i]->type == AST_EXPR_IDENTIFIER ||
		    array[i]->type == AST_EXPR_NUMBER ||
		    array[i]->type == AST_EXPR_CALL) {
			stack[index] = array[i];
			index++;
		} else if (array[i]->type == AST_EXPR_UNARY) {
			int priority = get_punctuator_priority(
				array[i]->value.unary.oparator);
			if (priority > flag) {
				stack[index] = array[i];
				index++;
				flag = priority;
			} else {
				index -= 2;
				stack[index]->value.unary.left =
					stack[index - 1];
				stack[index]->value.unary.right =
					stack[index + 1];
				stack[index - 1] = stack[index];
			}
		}
	}

	while (index > 2) {
		index -= 2;
		stack[index]->value.unary.left = stack[index - 1];
		stack[index]->value.unary.right = stack[index + 1];
		stack[index - 1] = stack[index];
	}

	return stack[0];
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

/*
 * 这个函数主要是创建函数调用的节点
 * 比如 add(a, 1) 这个函数的节点大概长这样
 * 	       call
 * 	    /    |   \
 *        add    a    1
 */
struct AST_expr *create_call_expr(struct token **tokens)
{
	struct AST_expr *call =
		(struct AST_expr *)alloc_memory(sizeof(struct AST_expr));
	call->type = AST_EXPR_CALL;
	call->value.call.name =
		(struct AST_expr *)alloc_memory(sizeof(struct AST_expr));
	call->value.call.name->type = AST_EXPR_IDENTIFIER;
	call->value.call.name->value.identifier = (*tokens)->value;

	*tokens = (*tokens)->next;

	int count = 0;
	struct token *temp = *tokens;
	while (temp->type != TOKEN_PUNCTUATOR_RIGHT_PARENTHESIS) {
		if (temp->type == TOKEN_IDENTIFIER ||
		    temp->type == TOKEN_CONSTANT_INTEGER) {
			count++;
		}
		temp = temp->next;
	}

	call->value.call.argc = count;
	call->value.call.args = (struct AST_expr **)alloc_memory(
		sizeof(struct AST_expr *) * count);

	int index = 0;
	while ((*tokens)->type != TOKEN_PUNCTUATOR_RIGHT_PARENTHESIS) {
		if ((*tokens)->type == TOKEN_IDENTIFIER) {
			call->value.call.args[index] =
				(struct AST_expr *)alloc_memory(
					sizeof(struct AST_expr));
			call->value.call.args[index]->type =
				AST_EXPR_IDENTIFIER;
			call->value.call.args[index]->value.identifier =
				(*tokens)->value;
			index++;
		} else if ((*tokens)->type == TOKEN_CONSTANT_INTEGER) {
			call->value.call.args[index] =
				(struct AST_expr *)alloc_memory(
					sizeof(struct AST_expr));
			call->value.call.args[index]->type = AST_EXPR_NUMBER;
			call->value.call.args[index]->value.number =
				atoi((*tokens)->value);
			index++;
		}
		*tokens = (*tokens)->next;
	}

	return call;
}

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

	// 将表达式节点存储到数组中 用于后续的表达式解析
	struct AST_expr **array = (struct AST_expr **)alloc_memory(
		sizeof(struct AST_expr *) * 64);
	int array_index = 0;

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
			array[array_index] = (struct AST_expr *)alloc_memory(
				sizeof(struct AST_expr));
			array[array_index]->type = AST_EXPR_KEYWORD;
			array[array_index]->value.keyword = tokens->value;
			array_index++;
		} else if (tokens->type == TOKEN_KEYWORD_RETURN) {
			// 关键字 return
			array[array_index] = (struct AST_expr *)alloc_memory(
				sizeof(struct AST_expr));
			array[array_index]->type = AST_EXPR_KEYWORD;
			array[array_index]->value.keyword = tokens->value;
		} else if (tokens->type == TOKEN_IDENTIFIER) {
			// 标识符
			if (tokens->next->type ==
			    TOKEN_PUNCTUATOR_LEFT_PARENTHESIS) {
				// 标识符 add 函数调用
				array[array_index] = create_call_expr(&tokens);
				array_index++;
			} else {
				// 标识符 a b
				array[array_index] =
					(struct AST_expr *)alloc_memory(
						sizeof(struct AST_expr));
				array[array_index]->type = AST_EXPR_IDENTIFIER;
				array[array_index]->value.identifier =
					tokens->value;
				array_index++;
			}
		} else if (tokens->type == TOKEN_CONSTANT_INTEGER) {
			// 常量 1 2
			array[array_index] = (struct AST_expr *)alloc_memory(
				sizeof(struct AST_expr));
			array[array_index]->type = AST_EXPR_NUMBER;
			array[array_index]->value.number = atoi(tokens->value);
			array_index++;
		} else if (tokens->type == TOKEN_PUNCTUATOR_ASSIGN ||
			   tokens->type == TOKEN_PUNCTUATOR_ADD ||
			   tokens->type == TOKEN_PUNCTUATOR_SUBTRACT ||
			   tokens->type == TOKEN_PUNCTUATOR_MULTIPLY ||
			   tokens->type == TOKEN_PUNCTUATOR_DIVIDE) {
			// = + - * /
			array[array_index] = (struct AST_expr *)alloc_memory(
				sizeof(struct AST_expr));
			array[array_index]->type = AST_EXPR_UNARY;
			array[array_index]->value.unary.oparator =
				tokens->value;
			array_index++;
		} else if (tokens->type == TOKEN_PUNCTUATOR_SEMICOLON) {
			// ;
			root->value.root.function[current_function_index]
				->value.function.body->value.body
				.expr[current_body_index] =
				array_parsing_to_tree(array, array_index);
			array_index = 0;
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
			array_index = 0;
		} else if (tokens->type == TOKEN_EOF) {
			break;
		}
		tokens = tokens->next;
	}

	return root;
}
