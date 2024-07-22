#include "parser.h"
#include "semantics.h"
#include "asm.h"
#include "utils.h"

static char *start_code = ".section .text\n"
			  ".global _start\n"
			  "_start:\n"
			  "\t# 调用 main 函数\n"
			  "\tcall main\n"
			  "\n"
			  "\t# 打印main函数的返回值\n"
			  "\t# %r10寄存器存储除数10\n"
			  "\tmovq $10, %r10\n"
			  "\n"
			  "\t# %rsi寄存器存储字符串起始地址 不包含'-'\n"
			  "\tleaq (%rsp), %rsi\n"
			  "\n"
			  "\t# %rcx存储字符串的长度\n"
			  "\txor %rcx, %rcx\n"
			  "\n"
			  "\ttest %rax, %rax\n"
			  "\tjl .is_negative_number\n"
			  "\tjmp .to_string\n"
			  "\n"
			  ".is_negative_number:\n"
			  "\tneg %rax\n"
			  "\tmovb $'-', (%rsp, %rcx)\n"
			  "\tincq %rcx\n"
			  "\tincq %rsi\n"
			  "\n"
			  ".to_string:\n"
			  "\t# %rdx 存储divq的余数\n"
			  "\txor %rdx, %rdx\n"
			  "\tdivq %r10\n"
			  "\taddq $48, %rdx\n"
			  "\tmovb %dl, (%rsp, %rcx)\n"
			  "\tincq %rcx\n"
			  "\ttest %rax, %rax\n"
			  "\tjne .to_string\n"
			  "\n"
			  "\tleaq (%rsp), %rdi\n"
			  "\taddq %rcx, %rdi\n"
			  "\tdecq %rdi\n"
			  "\n"
			  "\tmovb $'\\n', (%rsp, %rcx)\n"
			  "\tincq %rcx\n"
			  "\tjmp .reverse_string\n"
			  "\n"
			  ".reverse_string:\n"
			  "\tmovb (%rsi), %al\n"
			  "\tmovb (%rdi), %bl\n"
			  "\tmovb %bl, (%rsi)\n"
			  "\tmovb %al, (%rdi)\n"
			  "\tincq %rsi\n"
			  "\tdecq %rdi\n"
			  "\tcmpq %rsi, %rdi\n"
			  "\tjg .reverse_string\n"
			  "\n"
			  "\t# 系统调用将缓冲区中的字符串输出到标准输出\n"
			  "\tmovq $1, %rax\n"
			  "\tmovq $1, %rdi\n"
			  "\tleaq (%rsp), %rsi\n"
			  "\tmovq %rcx, %rdx\n"
			  "\tsyscall\n"
			  "\n"
			  "\t# 退出程序\n"
			  "\tmovq $60, %rax\n"
			  "\tmovq $0, %rdi\n"
			  "\tsyscall\n"
			  "\n\0";

static struct symbol_memory_layout *func_memory_layout;
static struct global_symbol_table *table;

int get_offset(const char *name)
{
	for (int i = 0; i < func_memory_layout->layout.func.argc; i++) {
		if (str_cmp(func_memory_layout->layout.func.args[i]->name,
			    name) == 0) {
			return func_memory_layout->layout.func.args[i]
				->layout.var.offset;
		}
	}
	return -1;
}

struct symbol_memory_layout *get_func_memory_layout(const char *name)
{
	struct symbol_memory_layout *layout =
		(struct symbol_memory_layout *)alloc_memory(
			sizeof(struct symbol_memory_layout));
	layout->name = name;
	int offset = 8;
	int start = -1, end = -1;
	for (int i = 0; i < table->count; i++) {
		if (table->symbols[i]->type == symbol_func) {
			if (str_cmp(table->symbols[i]->name, name) == 0) {
				start = i;
			} else if (start != -1) {
				end = i;
				break;
			}
		}
	}
	end = end == -1 ? table->count - 1 : end;
	int defined = 0;
	for (int i = start; i <= end; i++) {
		if (table->symbols[i]->type == symbol_func &&
		    table->symbols[i]->name == name) {
			defined += table->symbols[i]->attributes.func.argc;
		}
		if (table->symbols[i]->type == symbol_var &&
		    table->symbols[i]->status == symbol_status_defined) {
			defined++;
		}
	}
	layout->layout.func.argc = defined;
	layout->layout.func.args = (struct symbol_memory_layout **)alloc_memory(
		sizeof(struct symbol_memory_layout *) * defined);

	for (int i = start, index = 0; i <= end; i++) {
		if (table->symbols[i]->type == symbol_func &&
		    table->symbols[i]->name == name) {
			int argc = table->symbols[i]->attributes.func.argc;
			struct symbol **args =
				table->symbols[i]->attributes.func.args;
			for (int j = 0; j < argc; j++) {
				layout->layout.func.args
					[index] = (struct symbol_memory_layout *)
					alloc_memory(sizeof(
						struct symbol_memory_layout));
				layout->layout.func.args[index]->name =
					args[j]->name;
				layout->layout.func.args[index]
					->layout.var.offset = offset;
				layout->layout.func.args[index]
					->layout.var.func = layout;
				offset += 8;
				index++;
			}
		}
		if (table->symbols[i]->type == symbol_var &&
		    table->symbols[i]->status == symbol_status_defined) {
			layout->layout.func.args[index] =
				(struct symbol_memory_layout *)alloc_memory(
					sizeof(struct symbol_memory_layout));
			layout->layout.func.args[index]->name =
				table->symbols[i]->name;
			layout->layout.func.args[index]->layout.var.offset =
				offset;
			layout->layout.func.args[index]->layout.var.func =
				layout;
			offset += 8;
			index++;
		}
	}
	return layout;
}

static int stack[16];
static int index;
static int deepin;

void traverse_AST(struct AST_expr *expr, char *code)
{
	if (expr->type == AST_EXPR_ROOT) {
		for (int i = 0; i < expr->value.root.count; i++) {
			traverse_AST(expr->value.root.function[i], code);
		}
	} else if (expr->type == AST_EXPR_FUNCTION) {
		func_memory_layout = get_func_memory_layout(
			expr->value.function.prototype->value.prototype.name
				->value.identifier);
		traverse_AST(expr->value.function.prototype, code);
		traverse_AST(expr->value.function.body, code);
		str_cat(code, "\tleave\n");
		str_cat(code, "\tret\n");
		str_cat(code, "\n");
	} else if (expr->type == AST_EXPR_PROTOTYPE) {
		str_cat(code, expr->value.prototype.name->value.identifier);
		str_cat(code, ":\n");
		str_cat(code, "\tpushq %rbp\n");
		str_cat(code, "\tmovq %rsp, %rbp\n");
		if (func_memory_layout->layout.func.argc > 0) {
			str_cat(code, "\tsubq $");
			str_cat(code,
				itoa(func_memory_layout->layout.func.argc * 8));
			str_cat(code, ", %rsp\n");
		}
		str_cat(code, "\n");
		if (expr->value.prototype.argc == 1) {
			str_cat(code, "\tmovq %rdi, -8(%rbp)\n");
			str_cat(code, "\n");
		} else if (expr->value.prototype.argc == 2) {
			str_cat(code, "\tmovq %rdi, -8(%rbp)\n");
			str_cat(code, "\tmovq %rsi, -16(%rbp)\n");
			str_cat(code, "\n");
		} else if (expr->value.prototype.argc == 3) {
			str_cat(code, "\tmovq %rdi, -8(%rbp)\n");
			str_cat(code, "\tmovq %rsi, -16(%rbp)\n");
			str_cat(code, "\tmovq %rdx, -24(%rbp)\n");
			str_cat(code, "\n");
		} else if (expr->value.prototype.argc == 4) {
			str_cat(code, "\tmovq %rdi, -8(%rbp)\n");
			str_cat(code, "\tmovq %rsi, -16(%rbp)\n");
			str_cat(code, "\tmovq %rdx, -24(%rbp)\n");
			str_cat(code, "\tmovq %rcx, -32(%rbp)\n");
			str_cat(code, "\n");
		}
	} else if (expr->type == AST_EXPR_BODY) {
		for (int i = 0; i < expr->value.body.count; i++) {
			index = 0;
			stack[index] = -1;
			deepin = 0;
			traverse_AST(expr->value.body.expr[i], code);
			str_cat(code, "\n");
		}
	} else if (expr->type == AST_EXPR_BINARY) {
		if (str_cmp(expr->value.binary.oparator, "=") == 0) {
			traverse_AST(expr->value.binary.right, code);
			str_cat(code, "\tmovq %rax, -");
			str_cat(code,
				itoa(get_offset(expr->value.binary.left->value
							.identifier)));
			str_cat(code, "(%rbp)\n");
			return;
		}
		int flag = 0;
		if (expr->value.binary.left->type == AST_EXPR_BINARY &&
		    expr->value.binary.right->type == AST_EXPR_BINARY) {
			deepin++;
			flag = 1;
		}

		if (expr->value.binary.left->type == AST_EXPR_BINARY ||
		    expr->value.binary.right->type != AST_EXPR_BINARY) {
			stack[++index] = -1;
			traverse_AST(expr->value.binary.left, code);
			stack[++index] = 1;
			traverse_AST(expr->value.binary.right, code);
		} else {
			stack[++index] = 1;
			traverse_AST(expr->value.binary.right, code);
			stack[++index] = -1;
			traverse_AST(expr->value.binary.left, code);
		}

		if (str_cmp(expr->value.binary.oparator, "+") == 0) {
			if (flag == 1) {
				str_cat(code, "\tpopq %rax\n");
			}
			if (stack[index] == -1) {
				str_cat(code, "\taddq %rbx, %rax\n");
				if (deepin > 0)
					str_cat(code, "\tpushq %rax\n");
			} else if (stack[index] == 1) {
				str_cat(code, "\taddq %rax, %rbx\n");
			}
			if (flag == 1) {
				deepin--;
			}
			index--;
		} else if (str_cmp(expr->value.binary.oparator, "-") == 0) {
			if (flag == 1) {
				str_cat(code, "\tpopq %rax\n");
			}
			str_cat(code, "\tsubq %rbx, %rax\n");
			if (stack[index] == -1) {
				if (deepin > 0)
					str_cat(code, "\tpushq %rax\n");
			}
			if (stack[index] == 1) {
				str_cat(code, "\tmovq %rax, %rbx\n");
			}
			if (flag == 1) {
				deepin--;
			}
			index--;
		} else if (str_cmp(expr->value.binary.oparator, "*") == 0) {
			if (flag == 1) {
				str_cat(code, "\tpopq %rax\n");
			}
			str_cat(code, "\timulq %rbx, %rax\n");
			if (stack[index] == -1) {
				if (deepin > 0)
					str_cat(code, "\tpushq %rax\n");
			}
			if (stack[index] == 1) {
				str_cat(code, "\tmovq %rax, %rbx\n");
			}
			if (flag == 1) {
				deepin--;
			}
			index--;
		} else if (str_cmp(expr->value.binary.oparator, "/") == 0) {
			if (flag == 1) {
				str_cat(code, "\tpopq %rax\n");
			}
			str_cat(code, "\txor %rdx, %rdx\n");
			str_cat(code, "\tdivq %rbx\n");
			if (stack[index] == -1) {
				if (deepin > 0)
					str_cat(code, "\tpushq %rax\n");
			}
			if (stack[index] == 1) {
				str_cat(code, "\tmovq %rax, %rbx\n");
			}
			if (flag == 1) {
				deepin--;
			}
			index--;
		}
	} else if (expr->type == AST_EXPR_NUMBER) {
		str_cat(code, "\tmovq $");
		str_cat(code, itoa(expr->value.number));
		if (stack[index] == -1)
			str_cat(code, ", %rax\n");
		else if (stack[index] == 1)
			str_cat(code, ", %rbx\n");
		index--;
	} else if (expr->type == AST_EXPR_IDENTIFIER) {
		str_cat(code, "\tmovq -");
		str_cat(code, itoa(get_offset(expr->value.identifier)));
		if (stack[index] == -1)
			str_cat(code, "(%rbp), %rax\n");
		else if (stack[index] == 1)
			str_cat(code, "(%rbp), %rbx\n");
		index--;
	} else if (expr->type == AST_EXPR_CALL) {
		for (int i = 0; i < expr->value.call.argc; i++) {
			if (expr->value.call.args[i]->type == AST_EXPR_NUMBER) {
				str_cat(code, "\tmovq $");
				str_cat(code, itoa(expr->value.call.args[i]
							   ->value.number));
				str_cat(code, ", ");
			} else if (expr->value.call.args[i]->type ==
				   AST_EXPR_IDENTIFIER) {
				str_cat(code, "\tmovq -");
				str_cat(code,
					itoa(get_offset(
						expr->value.call.args[i]
							->value.identifier)));
				str_cat(code, "(%rbp), ");
			}
			switch (i) {
			case 0:
				str_cat(code, "%rdi\n");
				break;
			case 1:
				str_cat(code, "%rsi\n");
				break;
			case 2:
				str_cat(code, "%rdx\n");
				break;
			case 3:
				str_cat(code, "%rcx\n");
				break;
			}
		}
		if (stack[index] == 1)
			str_cat(code, "\tpushq %rax\n");
		str_cat(code, "\tcall ");
		str_cat(code, expr->value.call.name->value.identifier);
		str_cat(code, "\n");
		if (stack[index] == 1) {
			str_cat(code, "\tmovq %rax, %rbx\n");
			str_cat(code, "\tpopq %rax\n");
		}
		index--;
	}
}

char *asm_code_generator(struct AST_expr *root,
			 struct global_symbol_table *global_symbol_table)
{
	table = global_symbol_table;
	char *code = (void *)alloc_memory(sizeof(char) * 8192);
	str_copy(code, start_code);
	traverse_AST(root, code);

	return code;
}
