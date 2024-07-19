#include "semantics.h"
#include "parser.h"
#include "utils.h"
#include "io.h"

static struct symbol *current_func = (void *)0;
static int current_func_index = 0;
static int is_symbol_status_defined = 0;

enum symbol_status get_symbol_status(struct symbol *symbol,
				     struct global_symbol_table *tables)
{
	if (symbol->type == symbol_var) {
		int argc = tables->symbols[current_func_index]
				   ->attributes.func.argc;
		struct symbol **args = tables->symbols[current_func_index]
					       ->attributes.func.args;
		for (int i = 0; i < argc; i++) {
			if (str_cmp(args[i]->name, symbol->name) == 0) {
				if (args[i]->status == symbol_status_defined)
					return symbol_status_cite;
				return symbol_status_undefined;
			}
		}

		for (int i = current_func_index; i < tables->count - 1; i++) {
			if (tables->symbols[i]->type == symbol_var) {
				if (str_cmp(tables->symbols[i]->name,
					    symbol->name) == 0) {
					if (tables->symbols[i]->status ==
						    symbol_status_defined &&
					    tables->symbols[i]
							    ->attributes.var
							    .offset <
						    symbol->attributes.var
							    .offset)
						return symbol_status_cite;
					return symbol_status_undefined;
				}
			}
		}
	} else if (symbol->type == symbol_call) {
		for (int i = 0; i < tables->count - 1; i++) {
			if (tables->symbols[i]->type == symbol_func &&
			    str_cmp(tables->symbols[i]->name, symbol->name) ==
				    0 &&
			    tables->symbols[i]->attributes.func.argc ==
				    symbol->attributes.call.argc) {
				return symbol_status_cite;
			}
		}
	}
	return symbol_status_undefined;
}

void generate_global_symbol_table(struct AST_expr *expr,
				  struct global_symbol_table *tables,
				  int offset)
{
	if (expr->type == AST_EXPR_ROOT) {
		for (int i = 0; i < expr->value.root.count; i++) {
			generate_global_symbol_table(
				expr->value.root.function[i], tables, 0);
		}
	} else if (expr->type == AST_EXPR_FUNCTION) {
		struct symbol *symbol =
			(struct symbol *)alloc_memory(sizeof(struct symbol));
		tables->symbols[tables->count++] = symbol;
		symbol->name = expr->value.function.prototype->value.prototype
				       .name->value.identifier;
		symbol->type = symbol_func;
		symbol->status = symbol_status_defined;
		symbol->attributes.func.args = (struct symbol **)alloc_memory(
			sizeof(struct symbol *) *
			expr->value.function.prototype->value.prototype.argc);
		symbol->attributes.func.argc =
			expr->value.function.prototype->value.prototype.argc;
		int argc = expr->value.function.prototype->value.prototype.argc;
		for (int i = 0; i < argc; i++) {
			symbol->attributes.func.args[i] =
				(struct symbol *)alloc_memory(
					sizeof(struct symbol));
			symbol->attributes.func.args[i]->name =
				expr->value.function.prototype->value.prototype
					.args[i]
					->value.identifier;
			symbol->attributes.func.args[i]->type = symbol_var;
			symbol->attributes.func.args[i]->status =
				symbol_status_defined;
			symbol->attributes.func.args[i]->attributes.var.offset =
				offset;
			symbol->attributes.func.args[i]->attributes.var.func =
				symbol;
		}
		current_func = symbol;
		current_func_index = tables->count - 1;
		generate_global_symbol_table(expr->value.function.body, tables,
					     offset + 1);
	} else if (expr->type == AST_EXPR_BODY) {
		for (int i = 0; i < expr->value.body.count; i++) {
			generate_global_symbol_table(expr->value.body.expr[i],
						     tables, offset + i);
		}
	} else if (expr->type == AST_EXPR_BINARY) {
		if (str_cmp(expr->value.binary.oparator, "=") == 0)
			is_symbol_status_defined = 1;
		generate_global_symbol_table(expr->value.binary.left, tables,
					     offset);
		is_symbol_status_defined = 0;
		generate_global_symbol_table(expr->value.binary.right, tables,
					     offset);
	} else if (expr->type == AST_EXPR_IDENTIFIER) {
		struct symbol *symbol =
			(struct symbol *)alloc_memory(sizeof(struct symbol));
		tables->symbols[tables->count++] = symbol;
		symbol->name = expr->value.identifier;
		symbol->type = symbol_var;
		symbol->attributes.var.offset = offset;
		symbol->attributes.var.func = current_func;
		symbol->status = is_symbol_status_defined ?
					 symbol_status_defined :
					 get_symbol_status(symbol, tables);
	} else if (expr->type == AST_EXPR_CALL) {
		struct symbol *symbol =
			(struct symbol *)alloc_memory(sizeof(struct symbol));
		tables->symbols[tables->count++] = symbol;
		symbol->name = expr->value.call.name->value.identifier;
		symbol->type = symbol_call;
		symbol->attributes.call.args = (struct symbol **)alloc_memory(
			sizeof(struct symbol *) * expr->value.call.argc);
		symbol->attributes.call.argc = expr->value.call.argc;
		symbol->attributes.call.func = current_func;
		symbol->attributes.call.offset = offset;
		symbol->status = get_symbol_status(symbol, tables);
		for (int i = 0; i < expr->value.call.argc; i++) {
			symbol->attributes.call.args[i] =
				(struct symbol *)alloc_memory(
					sizeof(struct symbol));
			symbol->attributes.call.args[i]->name =
				expr->value.call.args[i]->value.identifier;
			symbol->attributes.call.args[i]->type = symbol_var;
			symbol->attributes.call.args[i]->attributes.var.offset =
				offset;
			symbol->attributes.call.args[i]->attributes.var.func =
				current_func;
			symbol->attributes.call.args[i]->status =
				get_symbol_status(
					symbol->attributes.call.args[i],
					tables);
		}
	}
}

struct global_symbol_table *init_global_symbol_table()
{
	struct global_symbol_table *tables =
		(struct global_symbol_table *)alloc_memory(
			sizeof(struct global_symbol_table));
	tables->symbols =
		(struct symbol **)alloc_memory(sizeof(struct symbol *) * 1024);
	tables->count = 0;
	return tables;
}

struct global_symbol_table *semantic_analysis(struct AST_expr *root)
{
	struct global_symbol_table *tables = init_global_symbol_table();
	generate_global_symbol_table(root, tables, 0);
	print("\n语义分析结果:\n");
	for (int i = 0; i < tables->count; i++) {
		if (tables->symbols[i]->type == symbol_var) {
			print("变量: %s   作用域: %s   偏移量: %d   ",
			      tables->symbols[i]->name,
			      tables->symbols[i]->attributes.var.func->name,
			      tables->symbols[i]->attributes.var.offset);
			print("状态: %s\n",
			      tables->symbols[i]->status ==
					      symbol_status_defined ?
				      "定义" :
			      tables->symbols[i]->status == symbol_status_cite ?
				      "引用" :
				      "未定义");
		} else if (tables->symbols[i]->type == symbol_func) {
			print("\n函数: %s   ", tables->symbols[i]->name);
			print("状态: %s\n",
			      tables->symbols[i]->status ==
					      symbol_status_defined ?
				      "定义" :
			      tables->symbols[i]->status == symbol_status_cite ?
				      "引用" :
				      "未定义");
			for (int j = 0;
			     j < tables->symbols[i]->attributes.func.argc;
			     j++) {
				print("|---参数: %s   作用域: %s   偏移量: %d   ",
				      tables->symbols[i]
					      ->attributes.func.args[j]
					      ->name,
				      tables->symbols[i]
					      ->attributes.func.args[j]
					      ->attributes.var.func->name,
				      tables->symbols[i]
					      ->attributes.func.args[j]
					      ->attributes.var.offset);
				print("状态: %s\n",
				      tables->symbols[i]->attributes.func
							      .args[j]
							      ->status ==
						      symbol_status_defined ?
					      "定义" :
				      tables->symbols[i]->attributes.func
							      .args[j]
							      ->status ==
						      symbol_status_cite ?
					      "引用" :
					      "未定义");
			}
		} else if (tables->symbols[i]->type == symbol_call) {
			print("函数调用: %s   作用域: %s   偏移量: %d   ",
			      tables->symbols[i]->name,
			      tables->symbols[i]->attributes.call.func->name,
			      tables->symbols[i]->attributes.call.offset);
			print("状态: %s\n",
			      tables->symbols[i]->status ==
					      symbol_status_defined ?
				      "定义" :
			      tables->symbols[i]->status == symbol_status_cite ?
				      "引用" :
				      "未定义");
			for (int j = 0;
			     j < tables->symbols[i]->attributes.call.argc;
			     j++) {
				print("|参数: %s   作用域: %s   偏移量: %d   ",
				      tables->symbols[i]
					      ->attributes.call.args[j]
					      ->name,
				      tables->symbols[i]
					      ->attributes.call.args[j]
					      ->attributes.var.func->name,
				      tables->symbols[i]
					      ->attributes.call.args[j]
					      ->attributes.var.offset);
				print("状态: %s\n",
				      tables->symbols[i]->attributes.call
							      .args[j]
							      ->status ==
						      symbol_status_defined ?
					      "定义" :
				      tables->symbols[i]->attributes.call
							      .args[j]
							      ->status ==
						      symbol_status_cite ?
					      "引用" :
					      "未定义");
			}
		}
	}

	for (int i = 0; i < tables->count; i++) {
		if (tables->symbols[i]->type == symbol_var &&
		    tables->symbols[i]->status == symbol_status_undefined) {
			goto fail;
		}
		if (tables->symbols[i]->type == symbol_call) {
			if (tables->symbols[i]->status ==
			    symbol_status_undefined) {
				goto fail;
			}
			for (int j = 0;
			     j < tables->symbols[i]->attributes.call.argc;
			     j++) {
				if (tables->symbols[i]
					    ->attributes.call.args[j]
					    ->status ==
				    symbol_status_undefined) {
					goto fail;
				}
			}
		}
	}
	print("\n通过语义分析\n\n");
	return tables;
fail:
	print("\n未通过语义分析\n\n");
	return (void *)0;
}
