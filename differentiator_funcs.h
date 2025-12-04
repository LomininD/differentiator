#ifndef DIFFERENTIATOR_FUNCS_H
#define DIFFERENTIATOR_FUNCS_H

#include "differentiator_properties.h"
#include "tree_funcs.h"
#include "inout_funcs.h"
#include "optimizator.h"
#include "dmath.h"

const int name_table_size = 26;
const int preset_names_num = 1;

struct name_record
{
	char var;
	double value;
};

extern name_record name_table[];
extern name_record preset_names[];


void print_menu();
void ask_for_variable_values();
err_t process_calculating_value(node* root_node);
err_t process_calculating_partial_derivative(tree* tree_ptr);
node* differentiate_node(tree* tree_ptr, node* current_node_ptr, char diff_var);
const char* decode_operation_type_enum(diff_ops op);
err_t wrap_constants(tree* tree_ptr);
err_t delete_neutral_elements(tree* tree_ptr);
double calculate_node(tree* tree_ptr, node* current_node);
bool check_for_diff_var(node* current_node_ptr, char diff_var);

int hash_var(char var);
bool is_preset(char var);

node* differentiate_number_node(tree* tree_ptr, node* current_node_ptr);
node* differentiate_var_node(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_op_node(tree* tree_ptr, node* current_node_ptr, char diff_var);

#endif