#ifndef ASM_H
#define ASM_H

#include "parser.h"
#include "semantics.h"

struct symbol_memory_layout {
	const char *name;
	union {
		struct {
			int offset;
			struct symbol_memory_layout *func;
		} var;

		struct {
			int argc;
			struct symbol_memory_layout **args;
		} func;
	} layout;
};

char *asm_code_generator(struct AST_expr *root,
			 struct global_symbol_table *table);

#endif
