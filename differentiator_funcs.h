#ifndef  DOFFERENTIATOR_FUNCS_H
#define DOFFERENTIATOR_FUNCS_H

#include "tree_funcs.h"
#include "inout_funcs.h"

err_t read_formula(tree* tree);

cmd_t request_cmd(const tree* tree);
err_t request_tree_beginning(tree* tree);
err_t request_new_nodes(tree* tree, node* parent_node);


#endif