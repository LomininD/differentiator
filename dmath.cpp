#include "dmath.h"
#include "differentiator_funcs.h"
#include <assert.h>
#include <math.h>
#include "tex_dump.h"

// TODO - remove excess headers

diff_op_t possible_ops[] = {{"+",   ADD, differentiate_add,   calc_add,  rm_add_sub_node,	dump_add_sub},
							{"-",   SUB, differentiate_sub,   calc_sub,  rm_add_sub_node, 	dump_add_sub},
							{"*",   MUL, differentiate_mul,   calc_mul,  rm_mul_node,		dump_mul},
							{"/",   DIV, differentiate_div,   calc_div,  rm_div_node,		dump_div},
							{"sin", SIN, differentiate_sin,   calc_sin,  rm_default_node, 	dump_unary_func},
							{"cos", COS, differentiate_cos,   calc_cos,  rm_default_node, 	dump_unary_func},
							{"^",   POW, differentiate_pow,   calc_pow,  rm_pow_node, 		dump_pow},
							{"ln",  LN,  differentiate_ln,    calc_ln,   rm_default_node, 	dump_unary_func}};


#define assert_args assert(tree_ptr != NULL); assert(current_node_ptr != NULL)
#define d(NODE_PTR) differentiate_node(tree_ptr, NODE_PTR, diff_var)
#define c(NODE_PTR) copy_node(tree_ptr, NODE_PTR)
#define l_subtr current_node_ptr->left
#define r_subtr current_node_ptr->right
#define to_op(CMD)  (union data_t){.operation = CMD}
#define to_num(NUM) (union data_t){.number = NUM}
#define non(OPERATION, LEFT, RIGHT) create_and_initialise_node(OP, to_op(OPERATION), LEFT, RIGHT, NULL)
#define nnn(NUMBER, LEFT, RIGHT) create_and_initialise_node(NUM, to_num(NUMBER), LEFT, RIGHT, NULL)
#define compound_func(EXP, INSIDE_FUNC) non(MUL, EXP, d(INSIDE_FUNC))
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

	tree_ptr->size += 6;
	return non(DIV, 
					non(SUB, 
							 non(MUL, d(l_subtr), c(r_subtr)), 
							 non(MUL, c(l_subtr), d(r_subtr))), 
				   	non(POW, 
							 c(r_subtr), 
							 nnn(2, NULL, NULL)));
}

node* differentiate_sin(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 2;
	return compound_func(non(COS, NULL, c(r_subtr)), 
						 r_subtr);
}

node* differentiate_cos(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 4;
	return compound_func(non(MUL, 
								  nnn(-1, NULL, NULL), 
								  non(SIN, NULL, c(r_subtr))), 
						 r_subtr);
}

node* differentiate_pow(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	printf("DIFFERENTIATE NODE\n");

	if (check_for_diff_var(current_node_ptr->left,  diff_var) && 
		check_for_diff_var(current_node_ptr->right, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in both args\n");
		tree_ptr->size += 6;

		return non(MUL,
						c(current_node_ptr),
						non(ADD,
								 non(MUL,
								 		  d(r_subtr),
										  non(LN, NULL, c(l_subtr))),
								 non(MUL, 
								 		  c(r_subtr),
										  non(DIV, 
										  		   d(l_subtr),
												   c(l_subtr)))));
		
	}
	else if (check_for_diff_var(current_node_ptr->left, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in base of the degree\n");
		tree_ptr->size += 5;

		return compound_func(non(MUL,
									 c(r_subtr),  
									 non(POW, 
								 			  c(l_subtr), 
								 			  non(SUB, 
										  			   c(r_subtr), 
										  			   nnn(1, NULL, NULL)))), 
							 current_node_ptr->left);

	}
	else if (check_for_diff_var(current_node_ptr->right, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in exponent\n");

		tree_ptr->size += 3;

		return compound_func(non(MUL,											 
								 	  c(current_node_ptr), 						
				 				 	  non(LN, NULL, c(l_subtr))),
				 			 r_subtr);

	}
	else 
	{
		return c(current_node_ptr);
	}
}

node* differentiate_ln(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert_args;

	tree_ptr->size += 3;
	return compound_func(non(DIV, 
								  nnn(1, NULL, NULL), 
								  c(current_node_ptr->right)),   
						 current_node_ptr->right);
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
#undef compound_func


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

double calc_ln(double a, double b)
{
	return log(b);
}




bool rm_mul_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	assert(tree_ptr != NULL);
	assert(current_node != NULL);
	assert(normal_node != NULL);
	assert(neutral_node != NULL);

	printf_debug_msg("current_node = %p, normal_node = %p, neutral_node = %p\n", current_node, *normal_node, *neutral_node);

	if ((*neutral_node)->type != NUM) return false;

	bool has_neutral = false;
	if ((*neutral_node)->data.number == 1)
	{
		if (branch_dir == left) 		current_node->parent->left  = *normal_node;
		else if (branch_dir == right)   current_node->parent->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		if (branch_dir != root)
		{
			printf_debug_msg("current_node: %p, parent [%p] ->left = %p\n", current_node, current_node->parent, current_node->parent->left);
			printf_debug_msg("current_node: %p, parent [%p] ->right = %p\n", current_node, current_node->parent, current_node->parent->right);
		}
		else
			printf_debug_msg("current_node: %p, parent [%p] root = %p\n", current_node, current_node->parent, tree_ptr->root);

		(*normal_node)->parent = current_node->parent;
		*normal_node = NULL;
		has_neutral = true;
	}
	else if ((*neutral_node)->data.number == 0)
	{
		if (branch_dir == left) 		current_node->parent->left  = *neutral_node;
		else if (branch_dir == right)   current_node->parent->right = *neutral_node;
		else 							tree_ptr->root = *neutral_node;

		if (branch_dir != root)
		{
			printf_debug_msg("current_node: %p, parent [%p] ->left = %p\n", current_node,current_node ->parent, current_node->parent->left);
			printf_debug_msg("current_node: %p, parent [%p] ->right = %p\n", current_node, current_node->parent, current_node->parent->right);
		}
		else
			printf_debug_msg("current_node: %p, parent [%p] root = %p\n", current_node, current_node->parent, tree_ptr->root);

		(*neutral_node)->parent = current_node->parent;
		*neutral_node = NULL;
		has_neutral = true;
	}

	if (has_neutral)
	{
		size_t destroyed = destroy_node(current_node);
		tree_ptr->size -= destroyed;
		printf_debug_msg("rm_mul_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_add_sub_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	assert(tree_ptr != NULL);
	assert(current_node != NULL);
	assert(normal_node != NULL);
	assert(neutral_node != NULL);

	printf_debug_msg("rm_add_sub_node: current_node = %p, normal_node = %p, neutral_node = %p\n", current_node, *normal_node, *neutral_node);

	if ((*neutral_node)->type != NUM) return false;

	if ((*neutral_node)->data.number == 0)
	{
		if (branch_dir == left) 		current_node->parent->left  = *normal_node;
		else if (branch_dir == right)   current_node->parent->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		(*normal_node)->parent = current_node->parent;
		*normal_node = NULL;

		size_t destroyed = destroy_node(current_node);
		tree_ptr->size -= destroyed;
		printf_debug_msg("rm_add_sub_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_div_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	assert(tree_ptr != NULL);
	assert(current_node != NULL);
	assert(normal_node != NULL);
	assert(neutral_node != NULL);

	printf_debug_msg("current_node = %p, normal_node = %p, neutral_node = %p\n", current_node, *normal_node, *neutral_node);

	if ((*neutral_node)->type != NUM) return false;

	bool has_neutral = false;
	if ((*neutral_node)->data.number == 1 && current_node->right == *neutral_node)
	{
		if (branch_dir == left) 		current_node->parent->left  = *normal_node;
		else if (branch_dir == right)   current_node->parent->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		(*normal_node)->parent = current_node->parent;
		*normal_node = NULL;
		has_neutral = true;
	}
	else if ((*neutral_node)->data.number == 0)
	{
		if (*neutral_node == current_node->right)
		{
			printf_log_err("[from rm_div_node] -> division by zero\n");
			global_err_stat = error;
			return false;
		}

		if (branch_dir == left) 		current_node->parent->left  = *neutral_node;
		else if (branch_dir == right)   current_node->parent->right = *neutral_node;
		else 							tree_ptr->root = *neutral_node;

		(*neutral_node)->parent = current_node->parent;
		*neutral_node = NULL;
		has_neutral = true;
	}

	if (has_neutral)
	{
		size_t destroyed = destroy_node(current_node);
		tree_ptr->size -= destroyed;
		printf_debug_msg("rm_mul_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_pow_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	assert(tree_ptr != NULL);
	assert(current_node != NULL);
	assert(normal_node != NULL);
	assert(neutral_node != NULL);

	printf_debug_msg("current_node = %p, normal_node = %p, neutral_node = %p\n", current_node, *normal_node, *neutral_node);

	if ((*neutral_node)->type != NUM) return false;

	bool has_neutral = false;
	if ((*neutral_node)->data.number == 1)
	{
		if (branch_dir == left) 		current_node->parent->left  = *normal_node;
		else if (branch_dir == right)   current_node->parent->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		(*normal_node)->parent = current_node->parent;
		*normal_node = NULL;
		has_neutral = true;
	}
	else if ((*neutral_node)->data.number == 0)
	{
		if ((*normal_node)->type == NUM && (*normal_node)->data.number == 0)
		{
			printf_log_err("[from rm_pow_node] -> zero in base and zero in exponent is forbidden\n");
			global_err_stat = error;
			return false;
		}
		
		node* new_node = create_and_initialise_node(NUM, (union data_t){.number = 1}, NULL, NULL, current_node->parent);

		if (branch_dir == left) 		current_node->parent->left  = new_node;
		else if (branch_dir == right)   current_node->parent->right = new_node;
		else 							tree_ptr->root = new_node;

		tree_ptr->size += 1;

		has_neutral = true;
	}

	if (has_neutral)
	{
		size_t destroyed = destroy_node(current_node);
		tree_ptr->size -= destroyed;
		printf_debug_msg("rm_mul_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_default_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	return false;
}

