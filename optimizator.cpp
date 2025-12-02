#include "optimizator.h"
#include <assert.h>
#include "tree_funcs.h"
#include "dmath.h"
#include "differentiator_funcs.h"

static bool wrap_node(tree* tree_ptr, node* current_node);
static void remove_neutral_elements_from_node(tree* tree_ptr, node* current_node, dir_t branch_dir);
static void remove_excess_node(tree* tree_ptr, node* current_node, dir_t branch_dir);


void optimize_equation(tree* tree_ptr)
{
	size_t old_size = 0;
	size_t new_size = 1;

	while (old_size != new_size)
	{
		old_size = tree_ptr->size;
		wrap_constants(tree_ptr);
		delete_neutral_elements(tree_ptr);
		new_size = tree_ptr->size;
	}
}


err_t wrap_constants(tree* tree_ptr) // TODO - check return value EVERYWHERE
{
	assert(tree_ptr != NULL);
	
	printf_debug_msg("wrap_constants: began process\n");
	VERIFY_TREE(tree_ptr, error);

	wrap_node(tree_ptr, tree_ptr->root);

	VERIFY_TREE(tree_ptr, error);
	printf_debug_msg("wrap_constants: finished process\n");

	print_tree_dump(tree_ptr, "Wrapped tree view\n");

	return ok;
}

// returns false if no var is found
bool wrap_node(tree* tree_ptr, node* current_node)
{
	assert(tree_ptr != NULL);

	printf_debug_msg("wrap_node: wraping %p\n", current_node);

	if (current_node == NULL)
		return false;

	bool left_has_var = wrap_node(tree_ptr, current_node->left);
	bool right_has_var = wrap_node(tree_ptr, current_node->right);

	if (left_has_var || right_has_var)
		return true;

	if (current_node->type == VAR)
	{
		printf_debug_msg("wrap_node: node %p has variable\n", current_node);
		return true;
	}

	if (current_node->type == NUM) return false;

	if (current_node->data.operation == LN  || 
		current_node->data.operation == SIN ||
		current_node->data.operation == COS) return false; // TODO - we dont always need to wrap these funcs

	double node_val = calculate_node(tree_ptr, current_node);

	size_t destroyed_nodes = 0;

	destroyed_nodes += destroy_node(current_node->left);
	destroyed_nodes += destroy_node(current_node->right);

	current_node->left = NULL;
	current_node->right = NULL;
	current_node->type = NUM;
	current_node->data.number = node_val;

	tree_ptr->size -= destroyed_nodes;

	printf_debug_msg("wrap_node: done wrapping %p\n", current_node);

	return false;
}


err_t delete_neutral_elements(tree* tree_ptr)
{
	assert(tree_ptr != NULL);
	
	printf_debug_msg("delete_neutral_elements: began process\n");
	VERIFY_TREE(tree_ptr, error);

	remove_neutral_elements_from_node(tree_ptr, tree_ptr->root, root);

	VERIFY_TREE(tree_ptr, error);
	printf_debug_msg("delete_neutral_elements: finished process\n");
	print_tree_dump(tree_ptr, "Tree with removed neutral elements view\n");


	return ok;
}


void remove_neutral_elements_from_node(tree* tree_ptr, node* current_node, dir_t branch_dir)
{
	assert(tree_ptr != NULL);

	printf_debug_msg("remove_neutral_elements_from_node: checking %p\n", current_node);

	if (current_node == NULL) return;

	remove_neutral_elements_from_node(tree_ptr, current_node->left, left);
	remove_neutral_elements_from_node(tree_ptr, current_node->right, right);

	if (current_node->type == OP)
		remove_excess_node(tree_ptr, current_node, branch_dir);
}


void remove_excess_node(tree* tree_ptr, node* current_node, dir_t branch_dir)
{
	assert(tree_ptr != NULL);
	assert(current_node != NULL);

	printf_debug_msg("remove_excess_node: began processing %p\n", current_node);

	bool removed = false;

	for (int i = 0; i < op_count; i++)
	{
		if (current_node->data.operation == possible_ops[i].op)
		{
			if (current_node->left != NULL)
			{
				removed = (*possible_ops[i].optimizator_func)(tree_ptr, current_node, &current_node->right, &current_node->left, branch_dir);
				if (removed) return;
			}

			if (current_node->right != NULL)
			{
				removed = (*possible_ops[i].optimizator_func)(tree_ptr, current_node, &current_node->left, &current_node->right, branch_dir);
				if (removed) return;
			}
		}
	}
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


bool rm_unremovable_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir)
{
	return false;
}