#ifndef DMATH_H
#define DMATH_H

#include "tree_properties.h"
#include "tree_dump.h" 

node* differentiate_number_node(tree* tree_ptr, node* current_node_ptr);
node* differentiate_var_node(tree* tree_ptr, node* current_node_ptr, char diff_var);


#endif