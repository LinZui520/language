#ifndef SEMANTICS_H
#define SEMANTICS_H
#include "parser.h"

enum symbol_type { symbol_var, symbol_func, symbol_call };

enum symbol_status {
	symbol_status_cite,
	symbol_status_defined,
	symbol_status_undefined
};

struct symbol {
	const char *name;
	enum symbol_type type;
	enum symbol_status status;

	union {
		struct {
			int offset;
			struct symbol *func;
		} var;

		struct {
			int argc;
			struct symbol **args;
		} func;

		struct {
			int argc;
			struct symbol **args;
			int offset;
			struct symbol *func;
		} call;
	} attributes;
};

struct global_symbol_table {
	struct symbol **symbols;
	int count;
};

enum semantic_analysis_status {
	semantic_analysis_status_success,
	semantic_analysis_status_fail
};

enum semantic_analysis_status semantic_analysis(struct AST_expr *root);

#endif
