#include "dmath.h"
#include <assert.h>
#include "differentiator_funcs.h"


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

	switch(current_node_ptr->data.operation) // TODO - optimize
	{
		case ADD:
			diffed_node_ptr =  differentiate_add_sub(tree_ptr, current_node_ptr, diff_var, ADD);
			break;
		case SUB:
			diffed_node_ptr =  differentiate_add_sub(tree_ptr, current_node_ptr, diff_var, SUB);
			break;
		case MUL:
			diffed_node_ptr = differentiate_mul(tree_ptr, current_node_ptr, diff_var);
			break;
		case DIV:
			// differentiate_div()
			break;
		default:
	};

	return diffed_node_ptr;
}


#define d(NODE_PTR) differentiate_node(tree_ptr, NODE_PTR, diff_var)
#define c(NODE_PTR) copy_node(tree_ptr, NODE_PTR)
#define l_subtr current_node_ptr->left
#define r_subtr current_node_ptr->right
#define to_op(CMD) (union data_t){.operation = CMD}
#define nn(...) create_and_initialise_node(__VA_ARGS__)


node* differentiate_add_sub(tree* tree_ptr, node* current_node_ptr, char diff_var, diff_ops op)
{
	assert(tree_ptr != NULL);
	assert(current_node_ptr != NULL);

	tree_ptr->size += 1;
	return nn(OP, to_op(op), d(l_subtr), d(r_subtr), NULL);
}

node* differentiate_mul(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert(tree_ptr != NULL);
	assert(current_node_ptr != NULL);

	tree_ptr->size += 3;
	return nn(OP, to_op(ADD), nn(OP, to_op(MUL), d(l_subtr), c(r_subtr), NULL), \
							  nn(OP, to_op(MUL), c(l_subtr), d(r_subtr), NULL), NULL);
}

#undef d
#undef c
#undef l_subtr
#undef r_subtr
#undef to_op
