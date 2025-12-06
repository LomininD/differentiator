#include <assert.h>
#include "../header_files/optimizator.h"
#include "../header_files/tree_funcs.h"
#include "../header_files/dmath.h"
#include "../header_files/differentiator_funcs.h"
#include "../header_files/tex_dump.h"

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

		if (old_size != new_size) dump_intermediate_calculations(tree_ptr->root);
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
