#include <assert.h>
#include <math.h>
#include "../header_files/dmath.h"
#include "../header_files/differentiator_funcs.h"
#include "../header_files/tex_dump.h"


// TODO - remove excess headers
// TODO - compare normally
// TODO - add sqrt
// TODO - check double for finite values

diff_op_t possible_ops[] = {{"+",      ADD,    differentiate_add,    calc_add,     rm_add_node,	  dump_add_sub},
							{"-",      SUB,    differentiate_sub,    calc_sub,     rm_sub_node,   dump_add_sub},
							{"*",      MUL,    differentiate_mul,    calc_mul,     rm_mul_node,		  dump_mul},
							{"/",      DIV,    differentiate_div,    calc_div,     rm_div_node,		  dump_div},
							{"^",      POW,    differentiate_pow,    calc_pow,     rm_pow_node, 	  dump_pow},
							{"ln",     LN,     differentiate_ln,     calc_ln,      rm_ln_node,        dump_unary_func},
							{"sin",    SIN,    differentiate_sin,    calc_sin,     rm_default_node,   dump_unary_func},
							{"cos",    COS,    differentiate_cos,    calc_cos,     rm_default_node,   dump_unary_func},
							{"tg",     TG,     differentiate_tg,     calc_tg,      rm_default_node,   dump_unary_func},
							{"ctg",    CTG,    differentiate_ctg,    calc_ctg,     rm_default_node,   dump_unary_func},
							{"arcsin", ARCSIN, differentiate_arcsin, calc_arcsin,  rm_default_node,   dump_unary_func},
							{"arccos", ARCCOS, differentiate_arccos, calc_arccos,  rm_default_node,   dump_unary_func}};


#define ASSERT_ARGS assert(tree_ptr != NULL); assert(current_node_ptr != NULL)
#define d(NODE_PTR) differentiate_node(tree_ptr, NODE_PTR, diff_var)
#define c(NODE_PTR) copy_node(tree_ptr, NODE_PTR)
#define L_SUBTR current_node_ptr->left
#define R_SUBTR current_node_ptr->right
#define DIFF_L d(L_SUBTR)
#define DIFF_R d(R_SUBTR)
#define COPY_L c(L_SUBTR)
#define COPY_R c(R_SUBTR)
#define TO_OP(CMD)  (union data_t){.operation = CMD}
#define TO_NUM(NUM) (union data_t){.number = NUM}
#define NON(OPERATION, LEFT, RIGHT) create_and_initialise_node(OP, TO_OP(OPERATION), LEFT, RIGHT, NULL)
#define NUM_N(NUMBER) create_and_initialise_node(NUM, TO_NUM(NUMBER), NULL, NULL, NULL)
#define ADD_N(LEFT, RIGHT) NON (ADD, LEFT, RIGHT)
#define SUB_N(LEFT, RIGHT) NON (SUB, LEFT, RIGHT)
#define MUL_N(LEFT, RIGHT) NON (MUL, LEFT, RIGHT)
#define DIV_N(LEFT, RIGHT) NON (DIV, LEFT, RIGHT)
#define POW_N(LEFT, RIGHT) NON (POW, LEFT, RIGHT)
#define LN_N(NODE)         NON (LN,  NULL, NODE)
#define SIN_N(NODE)		   NON (SIN, NULL, NODE)
#define COS_N(NODE)		   NON (COS, NULL, NODE)
#define COMPOUND_FUNC(EXP, INSIDE_FUNC) NON(MUL, EXP, d(INSIDE_FUNC));
#define SIZE tree_ptr->size


node* differentiate_add(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 1;
	return ADD_N (DIFF_L, DIFF_R);
}

node* differentiate_sub(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 1;
	return SUB_N( DIFF_L, DIFF_R);
}

node* differentiate_mul(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 3;
	return ADD_N (MUL_N (DIFF_L, COPY_R),
				  MUL_N (COPY_L, DIFF_R));
}

node* differentiate_div(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 6;
	return DIV_N (SUB_N (MUL_N (DIFF_L, COPY_R), 
						 MUL_N (COPY_L, DIFF_R)), 
				  POW_N (COPY_R, 
				 		 NUM_N (2)));
}

node* differentiate_pow(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	if (check_for_diff_var(current_node_ptr->left,  diff_var) && 
		check_for_diff_var(current_node_ptr->right, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in both args\n");
		SIZE += 6;

		return MUL_N (c(current_node_ptr),
					  ADD_N (MUL_N (DIFF_R,
									LN_N (COPY_L)),
							 MUL_N (COPY_R,
									DIV_N (DIFF_L,
										   COPY_L))));
		
	}
	else if (check_for_diff_var(current_node_ptr->left, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in base of the degree\n");
		SIZE += 5;

		return COMPOUND_FUNC (MUL_N (COPY_R,  
									 POW_N (COPY_L, 
								 			SUB_N (COPY_R, 
										  		   NUM_N (1)))), 
							  L_SUBTR);

	}
	else if (check_for_diff_var(current_node_ptr->right, diff_var))
	{
		printf_debug_msg("differentiate_pow: diff var in exponent\n");

		SIZE += 3;

		return COMPOUND_FUNC (MUL_N (c(current_node_ptr), 						
				 				 	 LN_N (COPY_L)),
				 			  R_SUBTR);
	}
	else 
	{
		SIZE += 1;
		return NUM_N (0);
	}
}

node* differentiate_ln(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 3;
	return COMPOUND_FUNC (DIV_N (NUM_N (1), 
								 COPY_R),   
						  R_SUBTR);
}


node* differentiate_sin(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 2;
	return COMPOUND_FUNC (COS_N (COPY_R), 
						  R_SUBTR);
}

node* differentiate_cos(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 4;
	return COMPOUND_FUNC (MUL_N (NUM_N (-1), 
								 SIN_N (COPY_R)), 
						 R_SUBTR);
}


node* differentiate_tg(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 6;
	return COMPOUND_FUNC (DIV_N(
								  NUM_N (1),
								  POW_N (COS_N (COPY_R),
										 NUM_N (2))),
						  R_SUBTR);
}

node* differentiate_ctg(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 6;
	return COMPOUND_FUNC (DIV_N (NUM_N (-1),
								 POW_N (SIN_N (COPY_R),
										NUM_N (2))),
						 R_SUBTR);
}

node* differentiate_arcsin(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 9;
	return COMPOUND_FUNC (DIV_N (NUM_N (1),
								 POW_N (SUB_N (NUM_N(1),
											   POW_N (COPY_R,
													  NUM_N (2))),
										   	   NUM_N (0.5))),
						  R_SUBTR);
}


node* differentiate_arccos(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	ASSERT_ARGS;

	SIZE += 9;
	return COMPOUND_FUNC (DIV_N (NUM_N(-1),
								 POW_N (SUB_N (NUM_N (1),
											   POW_N (COPY_R,
													  NUM_N (2))),
										   	   NUM_N (0.5))),
						  R_SUBTR);
}


#undef ASSERT_ARGS
#undef d
#undef c
#undef L_SUBTR
#undef R_SUBTR
#undef DIFF_L
#undef DIFF_R
#undef COPY_L
#undef COPY_R
#undef TO_OP
#undef TO_NUM
#undef NUM_N
#undef NON
#undef ADD_N
#undef SUB_N
#undef MUL_N
#undef DIV_N
#undef POW_N
#undef LN_N
#undef SIN_N
#undef COS_N
#undef COMPOUND_FUNC
#undef SIZE


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

double calc_pow(double a, double b)
{
	return pow(a, b);
}

double calc_ln(double a, double b)
{
	return log(b);
}

double calc_sin(double a, double b)
{
	return sin(b);
}

double calc_cos(double a, double b)
{
	return cos(b);
}

double calc_tg(double a, double b)
{
	return tan(b);
}

double calc_ctg(double a, double b)
{
	return 1.0 / tan(b);
}

double calc_arcsin(double a, double b)
{
	return asin(b);
}

double calc_arccos(double a, double b)
{
	return acos(b);
}


#define ASSERT_ARGS assert(tree_ptr != NULL && current_node != NULL && normal_node != NULL && neutral_node != NULL);
#define CHECK_TYPE(TYPE) if ((*neutral_node)->type != TYPE) return false;
#define PARENT_NODE current_node->parent
#define VAL_IS(NUM) check_data_equality(*neutral_node, NUM)
#define CLEAR_NODES size_t destroyed = destroy_node(current_node); tree_ptr->size -= destroyed

bool rm_mul_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	ASSERT_ARGS;

	printf_debug_msg("current_node = %p, normal_node = %p, neutral_node = %p\n", current_node, *normal_node, *neutral_node);

	CHECK_TYPE(NUM);

	bool has_neutral = false;
	if (VAL_IS(1))
	{
		if (branch_dir == left) 		PARENT_NODE->left  = *normal_node;
		else if (branch_dir == right)   PARENT_NODE->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		(*normal_node)->parent = PARENT_NODE;
		*normal_node = NULL;
		has_neutral = true;
	}
	else if (VAL_IS(0))
	{
		if (branch_dir == left) 		PARENT_NODE->left  = *neutral_node;
		else if (branch_dir == right)   PARENT_NODE->right = *neutral_node;
		else 							tree_ptr->root = *neutral_node;

		(*neutral_node)->parent = PARENT_NODE;
		*neutral_node = NULL;
		has_neutral = true;
	}

	if (has_neutral)
	{
		CLEAR_NODES;
		printf_debug_msg("rm_mul_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_add_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	ASSERT_ARGS;

	printf_debug_msg("rm_add_node: current_node = %p, normal_node = %p, neutral_node = %p\n", 
													current_node, *normal_node, *neutral_node);

	CHECK_TYPE(NUM);

	if (VAL_IS(0))
	{
		if (branch_dir == left) 		PARENT_NODE->left  = *normal_node;
		else if (branch_dir == right)   PARENT_NODE->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		(*normal_node)->parent = PARENT_NODE;
		*normal_node = NULL;

		CLEAR_NODES;
		printf_debug_msg("rm_add_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_sub_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	ASSERT_ARGS;

	printf_debug_msg("rm_add_sub_node: current_node = %p, normal_node = %p, neutral_node = %p\n", 
													current_node, *normal_node, *neutral_node);

	CHECK_TYPE(NUM);

	if (*neutral_node == current_node->left) return false;

	if (VAL_IS(0))
	{
		if (branch_dir == left) 		PARENT_NODE->left  = *normal_node;
		else if (branch_dir == right)   PARENT_NODE->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		(*normal_node)->parent = PARENT_NODE;
		*normal_node = NULL;

		CLEAR_NODES;
		printf_debug_msg("rm_sub_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_div_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	ASSERT_ARGS;

	printf_debug_msg("current_node = %p, normal_node = %p, neutral_node = %p\n", current_node, *normal_node, *neutral_node);

	CHECK_TYPE(NUM);

	bool has_neutral = false;
	if (VAL_IS(1) && current_node->right == *neutral_node)
	{
		if (branch_dir == left) 		PARENT_NODE->left  = *normal_node;
		else if (branch_dir == right)   PARENT_NODE->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		(*normal_node)->parent = PARENT_NODE;
		*normal_node = NULL;
		has_neutral = true;
	}
	else if (VAL_IS(0))
	{
		if (*neutral_node == current_node->right)
		{
			printf_log_err("[from rm_div_node] -> division by zero\n");
			global_err_stat = error;
			return false;
		}

		if (branch_dir == left) 		PARENT_NODE->left  = *neutral_node;
		else if (branch_dir == right)   PARENT_NODE->right = *neutral_node;
		else 							tree_ptr->root = *neutral_node;

		(*neutral_node)->parent = PARENT_NODE;
		*neutral_node = NULL;
		has_neutral = true;
	}

	if (has_neutral)
	{
		CLEAR_NODES;
		printf_debug_msg("rm_mul_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_pow_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	ASSERT_ARGS;

	printf_debug_msg("current_node = %p, normal_node = %p, neutral_node = %p\n", current_node, *normal_node, *neutral_node);

	CHECK_TYPE(NUM);

	bool has_neutral = false;
	if (VAL_IS(1))
	{
		if (branch_dir == left) 		PARENT_NODE->left  = *normal_node;
		else if (branch_dir == right)   PARENT_NODE->right = *normal_node;
		else 							tree_ptr->root = *normal_node;

		(*normal_node)->parent = PARENT_NODE;
		*normal_node = NULL;
		has_neutral = true;
	}
	else if (VAL_IS(0))
	{
		if ((*normal_node)->type == NUM && check_data_equality(*normal_node, 0))
		{
			printf_log_err("[from rm_pow_node] -> zero in base and zero in exponent is forbidden\n");
			global_err_stat = error;
			return false;
		}
		
		node* new_node = create_and_initialise_node(NUM, (union data_t){.number = 1}, NULL, NULL, PARENT_NODE);

		if (branch_dir == left) 		PARENT_NODE->left  = new_node;
		else if (branch_dir == right)   PARENT_NODE->right = new_node;
		else 							tree_ptr->root = new_node;

		tree_ptr->size += 1;

		has_neutral = true;
	}

	if (has_neutral)
	{
		CLEAR_NODES;
		printf_debug_msg("rm_mul_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_ln_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	ASSERT_ARGS;

	printf_debug_msg("current_node = %p, normal_node = %p, neutral_node = %p\n", current_node, *normal_node, *neutral_node);

	CHECK_TYPE(VAR);

	bool has_neutral = false;
	if ((*neutral_node)->data.variable == 'e')
	{
		node* new_node = create_and_initialise_node(NUM, (union data_t){.number = 1}, NULL, NULL, PARENT_NODE);
		
		if (branch_dir == left) 		PARENT_NODE->left = new_node;
		else if (branch_dir == right)   PARENT_NODE->right = new_node;
		else 							tree_ptr->root = new_node;

		has_neutral = true;
	}

	if (has_neutral)
	{
		CLEAR_NODES;
		printf_debug_msg("rm_mul_node: removed neutral elements of %p \n", current_node);
		return true;
	}
	return false;
}


bool rm_default_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	return false;
}


#undef ASSERT_ARGS
#undef CHECK_TYPE
#undef PARENT_NODE
#undef VAL_IS
#undef CLEAR_NODES


bool check_data_equality(node* node_ptr, double b)
{
    assert(node_ptr != NULL);
    assert(isfinite(b));

    return is_equal(node_ptr->data.number, b);
}


bool is_equal(double a, double b)
{
    assert(isfinite(a));
    assert(isfinite(b));

    return fabs(a - b) < EPSILON; // 10e-3
}