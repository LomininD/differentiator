#ifndef DIFFERENTIATOR_FUNCS_H
#define DIFFERENTIATOR_FUNCS_H

#include "differentiator_properties.h"
#include "tree_funcs.h"
#include "inout_funcs.h"

void print_menu();
cmd_t request_cmd(const tree* tree);
err_t request_tree_beginning(tree* tree);
err_t request_new_nodes(tree* tree, node* parent_node);


#endif