#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include "../header_files/tex_dump.h"
#include "../header_files/tree_funcs.h"
#include "../header_files/differentiator_funcs.h"

// TODO - fix issue with extra pars in d/dx()
// TODO - replace vars with numbers

FILE* tex_ptr = NULL;
const char* output_file_name = "output.tex";
const char* open_file_cmd = "open output.pdf";

static void dump_node(node* node_ptr);
static void dump_vars();
static void dump_var_values();
static size_t count_nodes(node* node_ptr);

const size_t max_nodes_displayed = 45;
node* replacements[26] = {};
int replaced = 0;

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
							 "Вы ещё скажите спасибо, что умножение коммутативно",
							 "Переменную интегрирования можно обозначить любой буквой: $x$, $y$, й... ",
							 "Вот такой вот беспредельный монстр функция Дирихле: не имеет предела ни в одной точке",
							 "Ну всё, п***ц \\textit{(© A. Скубачевский)}",
							 "Так, я же нигде не обосрался? \\textit{(© A. Скубачевский)}",
							 "Ну вот, it's a trap! \\textit{(© A. Скубачевский)}",
							 "С*ка, у неё остаточный член! Но зато какой! \\textit{(© 2-е задание по матану)}"};

#define FPRINT(...) fprintf(tex_ptr, __VA_ARGS__)

void initialise_tex_file()
{
	srand(time(NULL));
	tex_ptr = fopen(output_file_name, "w");	
	FPRINT("\\documentclass{article}\n");
	FPRINT("\\setlength{\\parindent}{0pt}\n");
	FPRINT("\\usepackage{graphicx}\n");
	FPRINT("\\usepackage[T2A]{fontenc}\n");
	FPRINT("\\usepackage[utf8]{inputenc}\n");
	FPRINT("\\usepackage[russian]{babel}\n\n");
	FPRINT("\\usepackage{amsfonts}\n\n");
	FPRINT("\\usepackage[left=2cm, top=2cm, right=2cm, bottom=2cm]{geometry}\n\n");
	FPRINT("\\usepackage{breqn}");
	FPRINT("\\title{ Техосмотр функции с Александром Пéтровичем}\n");
	FPRINT("\\author{ Developed by LMD \\\\ Approved by злая кафедра высшей математики}\n");
	FPRINT("\\begin{document}\n");
	FPRINT("\\maketitle\n");
}

void fill_main_equation_preamble(tree* tree_ptr)
{
	FPRINT("\\pagebreak\n");
	FPRINT("\\section{Основное уравнение}\n");
	FPRINT("Итак, нам дан такой пример: \\\\ \n");
	FPRINT("\\begin{dmath*}[spread=10pt]\n");
	FPRINT("f\\left( ");
	dump_vars();
	FPRINT("\\right) = ");
	dump_node(tree_ptr->root);
}

void dump_vars()
{
	bool first_var = true;
	for (int i = 0; i < name_table_size; i++)
	{
		if (name_table[i].var != 0)
		{
			if (!first_var) FPRINT(", ");
			FPRINT("%c", name_table[i].var);
			first_var = false;
		}
	}
}

void dump_end_main_equation_preamble()
{
	FPRINT("\n\\end{dmath*}\n");
	FPRINT("Расчехляем дифференциатор и начинаем считать.\n\n");
}

void dump_replacements()
{
	if (replaced > 0)
	{
		for (int i = 0; i < replaced; i++)
		{
			FPRINT("\\begin{dmath*}[spread=10pt]\n");
			FPRINT("\\text{Где} \\quad \\mathfrak{%c} = ", 'A' + i);
			dump_node(replacements[i]);
			FPRINT("\\end{dmath*}\n");
		}
		replaced = 0;
	}
}


void dump_calculating_expression_value(node* node_ptr, double value)
{
	FPRINT("Посчитаем значение выражения: \n\n");

	dump_var_values();

	FPRINT("\\begin{dmath*}\n");

	dump_node(node_ptr);

	FPRINT("= %lg\n", value);

	FPRINT("\\end{dmath*}\n\n");

	dump_replacements();
}

void dump_var_values()
{
	FPRINT("\\begin{gather*}\n");
	for (int i = 0; i < name_table_size; i++)
	{
		if (name_table[i].var != 0)
		{
			FPRINT("%c = %lg \\\\ \n", name_table[i].var, name_table[i].value);
		}
	}
	FPRINT("\\end{gather*}\n\n");
}


size_t count_nodes(node* node_ptr)
{
	int count = 1;

	if (node_ptr == NULL) return 0;

	count += count_nodes(node_ptr->left);
	count += count_nodes(node_ptr->right);

	return count;
}

#define CUR_OP node_ptr->data.operation
#define PARENT_OP node_ptr->parent->data.operation

void dump_node(node* node_ptr)
{
	assert(node_ptr != NULL);

	bool need_p = false;

	switch(node_ptr->type)
	{
		case NUM:
			if (node_ptr->data.number < 0 && (node_ptr->parent != NULL && PARENT_OP != DIV)) need_p = true;
			if (need_p) FPRINT("\\left( ");
			FPRINT("%lg ", node_ptr->data.number);
			if (need_p) FPRINT("\\right) ");
			break;
		case VAR:
			FPRINT("%c ", node_ptr->data.variable);
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
	FPRINT("\\pagebreak\n");
	FPRINT("\\section{Расчет производной}\n\n");
}

void dump_start_of_differentiation(node* node_ptr, char diff_var)
{
	FPRINT("\\begin{dmath*}[spread=10pt]\n");
	FPRINT("\\frac{d}{d%c} \\left( ", diff_var);
	dump_node(node_ptr);
	FPRINT("\\right)");
}

void fill_tangent_preamble()
{
	FPRINT("\\pagebreak\n");
	FPRINT("\\section{Расчет касательной}\n\n");
	FPRINT("Теперь найдём касательную к графику в точке:\n\n");
	FPRINT("\\begin{dmath*}[spread=10pt]\n");
	FPRINT("y_{\\text{кас}} = y(x_0) + y'(x_0)(x-x_0)\n");
	FPRINT("\\end{dmath*}\n\n");
}

void dump_intermediate_calculations(node* node_ptr)
{
	FPRINT("= ");
	dump_node(node_ptr);
}

void dump_end_of_differentiation()
{
	FPRINT("\n\\end{dmath*}\n\n");
	dump_replacements();
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

	FPRINT("%s \\\\ \n\n", phrase_bank[ind]);
	printf("%d\n", ind);
}

void close_tex_file()
{
	FPRINT("\\end{document}\n");
	fclose(tex_ptr);
}

// TODO - change long expr with letters
//----------------------------------------- DUMP OPERATIONS -------------------------------------------

#define OPEN_PAR  if (need_p) FPRINT("\\left( ")
#define CLOSE_PAR if (need_p) FPRINT("\\right) ")
#define CHECK_FOR_OUTER_PARS if (node_ptr->parent == NULL) need_p = false

void dump_add_sub(node* node_ptr, diff_op_t* op_struct)
{
	bool need_p = false;
	if (node_ptr->parent != NULL && node_ptr->parent->type == OP 
								 && (PARENT_OP == MUL || 
								 	 PARENT_OP == SUB || 
								 	 (PARENT_OP == POW && 
									 node_ptr == node_ptr->parent->left))) need_p = true;

	OPEN_PAR;
	dump_node(node_ptr->left);
	FPRINT("%s ", op_struct->name);
	dump_node(node_ptr->right);
	CLOSE_PAR;
}


void dump_mul(node* node_ptr, diff_op_t* op_struct)
{
	bool need_p = true;
	//bool need_dot = false;
	if (node_ptr->parent != NULL && node_ptr->parent->type == OP 
								 && (PARENT_OP == MUL || 
								 	 PARENT_OP == ADD || 
									 PARENT_OP == SUB ||
									 PARENT_OP == DIV)) need_p = false;

	// if (node_ptr->right->type == NUM || 
	// 			(node_ptr->right->data.operation == MUL && 
	// 							node_ptr->right->left->type == NUM)) need_dot = true;
	CHECK_FOR_OUTER_PARS;
	OPEN_PAR;
	dump_node(node_ptr->left);
	FPRINT("\\cdot ");
	dump_node(node_ptr->right);
	CLOSE_PAR;
}

void dump_div(node* node_ptr, diff_op_t* op_struct)
{
	bool need_p = false;
	if (node_ptr->parent != NULL && node_ptr->parent->type == OP 
								 && (PARENT_OP == POW)) need_p = true;

	OPEN_PAR;
	FPRINT("\\frac{ ");

	if (count_nodes(node_ptr->left) >= max_nodes_displayed)
	{
		char replacement_char = 'A' + replaced; 
		replacements[replaced] = node_ptr->left;
		replaced++;
		FPRINT("\\mathfrak{%c} ", replacement_char);
	}
	else
	{
		dump_node(node_ptr->left);
	}

	FPRINT("}{ ");

	if (count_nodes(node_ptr->right) >= max_nodes_displayed)
	{
		char replacement_char = 'A' + replaced; 
		replacements[replaced] = node_ptr->right;
		replaced++;
		FPRINT("\\mathfrak{%c} ", replacement_char);
	}
	else
	{
		dump_node(node_ptr->right);
	}

	FPRINT("} ");
	CLOSE_PAR;
}

void dump_unary_func(node* node_ptr, diff_op_t* op_struct)
{
	bool need_p = false;
	bool func_needs_p = false;

	if (node_ptr->parent != NULL && PARENT_OP == POW) func_needs_p = true;
	if (node_ptr->right->type == OP) need_p = true;
	if (func_needs_p) FPRINT("\\left( ");
	FPRINT("\\%s{ ", op_struct->name);

	OPEN_PAR;
	dump_node(node_ptr->right);
	CLOSE_PAR;

	FPRINT("} ");
	if (func_needs_p) FPRINT("\\right) ");
}

void dump_pow(node* node_ptr, diff_op_t* op_struct)
{
	bool need_p = false;
	if (node_ptr->parent != NULL && node_ptr->parent->type == OP 
								 && (PARENT_OP == POW)) need_p = true;
	OPEN_PAR;
	dump_node(node_ptr->left);
	FPRINT("^{ ");
	dump_node(node_ptr->right);
	FPRINT("} ");
	CLOSE_PAR;
}

#undef PARENT_OP
#undef OPEN_PAR
#undef CLOSE_PAR
#undef FPRINT
#undef CUR_OP

void convert_to_pdf()
{
    char sys_str[sys_str_size] = {};
    snprintf(sys_str, sys_str_size, "pdflatex  %s ", output_file_name); 
	// -interaction=batchmode > /dev/null 2>&1
    //puts(sys_str);

    system(sys_str);
	system(open_file_cmd);
}



