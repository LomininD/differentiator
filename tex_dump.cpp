#include "tex_dump.h"
#include <stdlib.h>
#include "tree_funcs.h"
#include <assert.h>

FILE* tex_ptr = NULL;
const char* output_file_name = "output.tex";
const char* open_file_cmd = "open output.pdf";

static void dump_tree_in_tex(tree* tree_ptr);
static void dump_node(node* node_ptr);


#define fprint(...) fprintf(tex_ptr, __VA_ARGS__)

void initialise_tex_file()
{
	tex_ptr = fopen(output_file_name, "w");	
	fprint("\\documentclass{article}\n");
	fprint("\\usepackage{graphicx}\n");
	fprint("\\usepackage[T2A]{fontenc}\n");
	fprint("\\usepackage[utf8]{inputenc}\n");
	fprint("\\usepackage[russian]{babel}\n\n");
	fprint("\\usepackage{breqn}");
	fprint("\\title{ Отчёт дифференциатора }\n");
	fprint("\\author{ Developed by LMD }\n");
	fprint("\\begin{document}\n");
	fprint("\\maketitle\n");
}

void fill_main_equation_preamble(tree* tree_ptr)
{
	fprint("\\pagebreak\n");
	fprint("\\section{Основное уравнение}\n");
	fprint("Итак, нам дан такой пример: \\\\ \n");
	fprint("\\begin{dmath*}\n");
	dump_tree_in_tex(tree_ptr);
}

void dump_end_main_equation_preamble()
{
	fprint("\n\\end{dmath*}\n");
	fprint("Расчехляем дифференциатор и начинаем считать.\n");
}

void dump_tree_in_tex(tree* tree_ptr)
{
	VERIFY_TREE(tree_ptr,); // !!!

	dump_node(tree_ptr->root);
}

#define cur_op node_ptr->data.operation

void dump_node(node* node_ptr)
{
	assert(node_ptr != NULL);

	switch(node_ptr->type)
	{
		case NUM:
			fprint("%lg ", node_ptr->data.number);
			break;
		case VAR:
			fprint("%c ", node_ptr->data.variable);
			break;
		case OP:
			for(int i = 0; i < op_count; i++)
			{
				if (node_ptr->data.operation == possible_ops[i].op)
				{
					//printf_debug_msg("OPERATION: %d\n", node_ptr->data.operation);
					(*(possible_ops[i].tex_dump_func))(node_ptr, &(possible_ops[i]));
				}
			}
	};
}

#undef cur_op

void fill_derivative_preamble()
{
	fprint("\\pagebreak\n");
	fprint("\\section{Расчет производной}\n");
	fprint("Здесь будет считаться производная\n");
}

void dump_start_of_differentiation(tree* tree_ptr, char diff_var)
{
	fprint("\\begin{dmath*}\n");
	fprint("\\frac{d}{d%c} \\left( ", diff_var);
	dump_tree_in_tex(tree_ptr);
	fprint("\\right)");
}

void dump_intermediate_calculations(tree* tree_ptr)
{
	fprint("= ");
	dump_tree_in_tex(tree_ptr);
}

void dump_end_of_differentiation()
{
	fprint("\n\\end{dmath*}\n\n");
}

void close_tex_file()
{
	fprint("\\end{document}\n");
	fclose(tex_ptr);
}

//----------------------------------------- DUMP OPERATIONS -------------------------------------------

void dump_add_sub(node* node_ptr, diff_op_t* op_struct)
{
	bool need_p = true;
	if (node_ptr->parent != NULL && node_ptr->parent->type == OP 
								 && node_ptr->parent->data.operation == DIV) need_p = false;
	if (need_p) fprint("\\left( ");
	dump_node(node_ptr->left);
	fprint("%s ", op_struct->name);
	dump_node(node_ptr->right);
	if (need_p) fprint("\\right) ");
}

void dump_mul(node* node_ptr, diff_op_t* op_struct)
{
	fprint("\\left( ");
	dump_node(node_ptr->left);
	//if (node_ptr->right->type = NUM) fprint("\\cdot ");
	dump_node(node_ptr->right);
	fprint("\\right) ");
}

void dump_div(node* node_ptr, diff_op_t* op_struct)
{
	fprint("\\frac{ ");
	dump_node(node_ptr->left);
	fprint("}{ ");
	dump_node(node_ptr->right);
	fprint("} ");
}

void dump_unary_func(node* node_ptr, diff_op_t* op_struct)
{
	fprint("\\%s{ ", op_struct->name);
	dump_node(node_ptr->right);
	fprint("} ");
}

void dump_pow(node* node_ptr, diff_op_t* op_struct)
{
	dump_node(node_ptr->left);
	fprint("^{ ");
	dump_node(node_ptr->right);
	fprint("} ");
}


#undef fprint

void convert_to_pdf()
{
    char sys_str[sys_str_size] = {};
    snprintf(sys_str, sys_str_size, "pdflatex %s", output_file_name); 

    //puts(sys_str);

    system(sys_str);
	system(open_file_cmd);
}



