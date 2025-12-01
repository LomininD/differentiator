#ifndef OPTIMIZATOR_H
#define OPTIMIZATOR_H

#include "differentiator_properties.h"

enum dir_t
{
	left,
	right,
	root,
};

err_t wrap_constants(tree* tree_ptr);
err_t delete_neutral_elements(tree* tree_ptr);
void optimize_equation(tree* tree_ptr);

bool rm_mul_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_add_sub_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_div_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_pow_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_unremovable_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);

#endif