#include "dmath.h"
#include <assert.h>
#include "differentiator_funcs.h"
#include <math.h>
#include "optimizator.h"

static bool check_for_diff_var(node* current_node_ptr, char diff_var);

// TODO - remove to other file
diff_op_t possible_ops[] = {{"+",   ADD, differentiate_add,   calc_add,  rm_add_sub_node},
							{"-",   SUB, differentiate_sub,   calc_sub,  rm_add_sub_node},
							{"*",   MUL, differentiate_mul,   calc_mul,  rm_mul_node},
							{"/",   DIV, differentiate_div,   calc_div,  rm_div_node},
							{"sin", SIN, differentiate_sin,   calc_sin,  rm_unremovable_node},
							{"cos", COS, differentiate_cos,   calc_cos,  rm_unremovable_node},
							{"^",   POW, differentiate_pow,   calc_pow,  rm_pow_node}};

#define check_for_mem_err(FUNC) { 															\
	if (diffed_node_ptr == NULL)															\
	{																						\
		printf_log_err("[from %s] -> could not allocate memory for new node\n", FUNC);		\
		global_err_stat = error;															\
		return NULL;																		\
	}																						\
}


node* differentiate_number_node(tree* tree_ptr, node* current_node_ptr)
{
	assert(tree_ptr != NULL);
	assert(current_node_ptr != NULL);

	node* diffed_node_ptr = (node*) calloc(1, sizeof(node));
	check_for_mem_err("differentiate_number_node");

	tree_ptr->size += 1;
	
	diffed_node_ptr->type = NUM;

	diffed_node_ptr->data.number = 0;

	return diffed_node_ptr;
}


node* differentiate_var_node(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert(tree_ptr != NULL);
	assert(current_node_ptr != NULL);

	node* diffed_node_ptr = (node*) calloc(1, sizeof(node));
	check_for_mem_err("differentiate_var_node");

	tree_ptr->size += 1;

	if (current_node_ptr->data.variable == diff_var)
	{
		diffed_node_ptr->type = NUM;
		diffed_node_ptr->data.number = 1;
	}
	else
	{
		diffed_node_ptr->type = VAR;
		diffed_node_ptr->data.variable = current_node_ptr->data.variable;
	}

	return diffed_node_ptr;
}


node* differentiate_op_node(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert(tree_ptr != NULL);
	assert(current_node_ptr != NULL);
	node* diffed_node_ptr = NULL;

	for (int i = 0; i < op_count; i++)
	{
		if (current_node_ptr->data.operation == possible_ops[i].op)
		{
			diffed_node_ptr = (*(possible_ops[i].diff_operation))(tree_ptr, current_node_ptr, diff_var);
		}
	}

	return diffed_node_ptr;
}


#define assert_args assert(tree_ptr != NULL); assert(current_node_ptr != NULL)
#define d(NODE_PTR) differentiate_node(tree_ptr, NODE_PTR, diff_var)
#define c(NODE_PTR) copy_node(tree_ptr, NODE_PTR)
#define l_subtr current_node_ptr->left
#define r_subtr current_node_ptr->right
#define to_op(CMD)  (union data_t){.operation = CMD}
#define to_num(NUM) (union data_t){.number = NUM}
#define non(OPERATION, LEFT, RIGHT) create_and_initialise_node(OP, to_op(OPERATION), LEFT, RIGHT, NULL)
#define nnn(NUMBER, LEFT, RIGHT) create_and_initialise_node(NUM, to_num(NUMBER), LEFT, RIGHT, NULL)
// TODO - remove left right

node* differentiate_add(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 1;
	return non(ADD, d(l_subtr), d(r_subtr));
}

node* differentiate_sub(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 1;
	return non(SUB, d(l_subtr), d(r_subtr));
}

node* differentiate_mul(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 3;
	return non(ADD, non(MUL, d(l_subtr), c(r_subtr)), \
					non(MUL, c(l_subtr), d(r_subtr)));
}

node* differentiate_div(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 5;
	return non(DIV, non(SUB, non(MUL, d(l_subtr), c(r_subtr)), non(MUL, c(l_subtr), d(r_subtr))), \
				   	non(MUL, c(r_subtr), c(r_subtr)));
}

node* differentiate_sin(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 2;
	return non(MUL, non(COS, NULL, c(r_subtr)), d(r_subtr));
}

node* differentiate_cos(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 4;
	return non(MUL, non(MUL, nnn(-1, NULL, NULL), non(SIN, NULL, c(r_subtr))), d(r_subtr));
}

node* differentiate_pow(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	printf("DIFFERENTIATE NODE\n");

	if (check_for_diff_var(current_node_ptr->left,  diff_var) && 
		check_for_diff_var(current_node_ptr->right, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in both args\n");
		return NULL; // TODO - fix after adding ln	
	}
	else if (check_for_diff_var(current_node_ptr->left, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in base of the degree\n");
		tree_ptr->size += 5;
		return non(MUL, c(current_node_ptr->right), 
		non(MUL, non(POW, c(current_node_ptr->left), non(SUB, c(current_node_ptr->right), nnn(1, NULL, NULL))), d(current_node_ptr->left)));
	}
	else if (check_for_diff_var(current_node_ptr->right, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in exponent\n");
		return NULL; // TODO - fix after adding ln
	}
	else 
	{
		return c(current_node_ptr);
	}
}


#undef assert_args
#undef d
#undef c
#undef l_subtr
#undef r_subtr
#undef to_op
#undef to_num
#undef nnn
#undef non


bool check_for_diff_var(node* current_node_ptr, char diff_var)
{
	if (current_node_ptr == NULL) return false;

	if (current_node_ptr->type == VAR)
	{
		if (current_node_ptr->data.variable == diff_var)
			return true;
	}
	return (check_for_diff_var(current_node_ptr->left,  diff_var) || 
			check_for_diff_var(current_node_ptr->right, diff_var));
}


double calc_add(double a, double b)
{
	return a + b;
}

double calc_sub(double a, double b)
{
	return a - b;
}

double calc_mul(double a, double b)
{
	return a * b;
}

double calc_div(double a, double b)
{
	return a / b;
}

double calc_sin(double a, double b)
{
	return sin(b);
}

double calc_cos(double a, double b)
{
	return cos(b);
}

double calc_pow(double a, double b)
{
	return pow(a, b);
}

