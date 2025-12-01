#ifndef DIFFERENTIATOR_FUNCS_H
#define DIFFERENTIATOR_FUNCS_H

#include "differentiator_properties.h"
#include "tree_funcs.h"
#include "inout_funcs.h"
#include "optimizator.h"

void print_menu();
err_t process_calculating_partial_derivative(tree* tree_ptr);
node* differentiate_node(tree* tree_ptr, node* current_node_ptr, char diff_var);
const char* decode_operation_type_enum(diff_ops op);
err_t wrap_constants(tree* tree_ptr);
err_t delete_neutral_elements(tree* tree_ptr);
double calculate_node(tree* tree_ptr, node* current_node);

#endif