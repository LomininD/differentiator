#include "tex_dump.h"
#include <stdlib.h>
#include "tree_funcs.h"
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include "differentiator_funcs.h"

FILE* tex_ptr = NULL;
const char* output_file_name = "output.tex";
const char* open_file_cmd = "open output.pdf";

static void dump_node(node* node_ptr);
static void dump_vars();

const char* phrase_bank[] = {"Любой уважающий себя синус трепыхается от -1 до 1.",
							 "Это выражение из логарифмов ни уму ни сердцу ничего не говорит",
							 "Единичка скончалась",
							 "Не занимайтесь членовредительством!",
							 "Открываем БАЛШОЙ, БАЛШОЙ КВАДРАТНЫЙ СКОБКА",
							 "Производная -- это, конечно, приятно, но зачем она нужна?!",
							 "Ну, вы же ясно видите, что расстояние между точками увеличилось в корень из двух раз.",
							 "Причёсываем это выражение",
							 "Куда ни ткни -- всё дрянь какая-то... Не надо только тереть, а то потом концов не соберём.",
							 "Очень приплюснутая штучка! Ан прямо интересно, какая приплюснутая!",
							 "Сейчас всё возрастает, а потом убывает, а потом опять возрастает, а потом.... чёрт его знает",
							 "Если вы не воспринимаете того, что я сейчас вам говорю.... а ну и бог с ним!",
							 "Этим дрючат студентов на третьем курсе, но это очень лёгкая вещь. Вот смотрите:",
							 "Подставили интеграл -- всё, трах-тарарах, скончалось!",
							 "Ну всё, п***ц \\textit{(© A. Скубачевский)}",
							 "С*ка, у неё остаточный член! Но зато какой! \\textit{(© 2-е задание по матану)}"};

#define fprint(...) fprintf(tex_ptr, __VA_ARGS__)

void initialise_tex_file()
{
	srand(time(NULL));
	tex_ptr = fopen(output_file_name, "w");	
	fprint("\\documentclass{article}\n");
	fprint("\\usepackage{graphicx}\n");
	fprint("\\usepackage[T2A]{fontenc}\n");
	fprint("\\usepackage[utf8]{inputenc}\n");
	fprint("\\usepackage[russian]{babel}\n\n");
	fprint("\\usepackage[left=2cm, top=2cm, right=2cm, bottom=2cm]{geometry}\n\n");
	fprint("\\usepackage{breqn}");
	fprint("\\title{ Техосмотр функции с Александром Пéтровичем}\n");
	fprint("\\author{ Developed by LMD }\n");
	fprint("\\begin{document}\n");
	fprint("\\maketitle\n");
}

void fill_main_equation_preamble(tree* tree_ptr)
{
	fprint("\\pagebreak\n");
	fprint("\\section{Основное уравнение}\n");
	fprint("Итак, нам дан такой пример: \\\\ \n");
	fprint("\\begin{dmath*}[spread=10pt]\n");
	fprint("f\\left( ");
	dump_vars();
	fprint("\\right) = ");
	dump_node(tree_ptr->root);
}

void dump_vars()
{
	bool first_var = true;
	for (int i = 0; i < name_table_size; i++)
	{
		if (name_table[i].var != 0)
		{
			if (!first_var) fprint(", ");
			fprint("%c", name_table[i].var);
			first_var = false;
		}
	}
}

void dump_end_main_equation_preamble()
{
	fprint("\n\\end{dmath*}\n");
	fprint("Расчехляем дифференциатор и начинаем считать.\n");
}


#define cur_op node_ptr->data.operation

void dump_node(node* node_ptr)
{
	assert(node_ptr != NULL);

	bool need_p = false;

	switch(node_ptr->type)
	{
		case NUM:
			if (node_ptr->data.number < 0 && node_ptr->parent != NULL) need_p = true;
			if (need_p) fprint("\\left( ");
			fprint("%lg ", node_ptr->data.number);
			if (need_p) fprint("\\right) ");
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



void fill_derivative_preamble()
{
	fprint("\\pagebreak\n");
	fprint("\\section{Расчет производной}\n\n");
}

void dump_start_of_differentiation(node* node_ptr, char diff_var)
{
	fprint("\\begin{dmath*}[spread=10pt]\n");
	fprint("\\frac{d}{d%c} \\left( ", diff_var);
	dump_node(node_ptr);
	fprint("\\right)");
}

void dump_intermediate_calculations(node* node_ptr)
{
	fprint("= ");
	dump_node(node_ptr);
}

void dump_end_of_differentiation()
{
	fprint("\n\\end{dmath*}\n\n");
}

void dump_text(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);

	vfprintf(tex_ptr, format, ap);

	va_end(ap);
}

void insert_random_phrase()
{
	double normalized = (double) rand() / (double) RAND_MAX;
	int ind = (int)(normalized * (double)(sizeof(phrase_bank) / (double)sizeof(const char*)));

	fprint("%s \\\\ \n\n", phrase_bank[ind]);
	printf("%d\n", ind);
}

void close_tex_file()
{
	fprint("\\end{document}\n");
	fclose(tex_ptr);
}

// TODO - change long expr with letters
//----------------------------------------- DUMP OPERATIONS -------------------------------------------

#define open_par  if (need_p) fprint("\\left( ")
#define close_par if (need_p) fprint("\\right) ")
#define parent_op node_ptr->parent->data.operation
#define check_for_outer_pars if (node_ptr->parent == NULL) need_p = false

void dump_add_sub(node* node_ptr, diff_op_t* op_struct)
{
	bool need_p = true;
	if (node_ptr->parent != NULL && node_ptr->parent->type == OP 
								 && (parent_op == DIV || parent_op == ADD)) need_p = false;

	check_for_outer_pars;

	open_par;
	dump_node(node_ptr->left);
	fprint("%s ", op_struct->name);
	dump_node(node_ptr->right);
	close_par;
}


void dump_mul(node* node_ptr, diff_op_t* op_struct)
{
	bool need_p = true;
	//bool need_dot = false;
	if (node_ptr->parent != NULL && node_ptr->parent->type == OP 
								 && (parent_op == MUL || 
								 	 parent_op == ADD || 
									 parent_op == SUB)) need_p = false;

	// if (node_ptr->right->type == NUM || 
	// 			(node_ptr->right->data.operation == MUL && 
	// 							node_ptr->right->left->type == NUM)) need_dot = true;
	check_for_outer_pars;
	open_par;
	dump_node(node_ptr->left);
	fprint("\\cdot ");
	dump_node(node_ptr->right);
	close_par;
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
	bool need_p = false;
	if (node_ptr->parent != NULL && node_ptr->parent->type == OP 
								 && (parent_op == POW)) need_p = true;
	open_par;
	dump_node(node_ptr->left);
	fprint("^{ ");
	dump_node(node_ptr->right);
	fprint("} ");
	close_par;
}

#undef parent_op
#undef open_par
#undef close_par
#undef fprint
#undef cur_op

void convert_to_pdf()
{
    char sys_str[sys_str_size] = {};
    snprintf(sys_str, sys_str_size, "pdflatex %s", output_file_name); 

    //puts(sys_str);

    system(sys_str);
	system(open_file_cmd);
}



