#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "../header_files/differentiator_funcs.h"
#include "../header_files/tex_dump.h"

#ifndef M_E
#define M_E 2.718281
#endif

name_record name_table[name_table_size] = {};
name_record preset_names[preset_names_num] = {{'e', M_E}};

static tree* differentiate_tree(const tree* old_tree_ptr, char diff_var);
static double calculate_value(node* node_ptr);
static bool determine_var_value(char var, name_record var_table[], int table_size, double* value);

// TODO - check if entered var is in name table

void print_menu()
{
	printf_both("-> Choose a command to execute:\n");
	printf("- [c]ount (not done yet)\n");
	printf("- [s]implify (not done yet)\n");
	printf("- [d]erivative (not done yet)\n");
	printf("- [p]artial derivative\n");
	printf("- [g]raph (not done yet)\n");
	printf("- [t]aylor polynomial (not done yet)\n");
	printf("- [q]uit\n");
}





void ask_for_variable_values()
{
	for (int i = 0; i < name_table_size; i++)
	{
		if (name_table[i].var != 0)
		{
			if (is_preset(name_table[i].var)) continue;
			printf_both("-> Enter %c value:\n", name_table[i].var);
			double value = request_number();
			name_table[i].value  = value;
		}
	}
}


bool is_preset(char var)
{
	for (int i = 0; i < preset_names_num; i++)
	{
		if (var == preset_names[i].var)
			return true;
	}
	return false;
}


err_t process_calculating_value(node* root_node)
{
	assert(root_node != NULL);
	printf_debug_msg("process_calculating_value: began process\n");

	double total_value = calculate_value(root_node);
	CHECK_ERR(error);

	printf_both("->function value is %lg\n", total_value);

	dump_calculating_expression_value(root_node, total_value);

	printf_debug_msg("process_calculating_value: finished process\n");

	return ok;
}

#define TYPE node_ptr->type

double calculate_value(node* node_ptr)
{
	if (node_ptr == NULL) return ok;

	if (TYPE == NUM) return node_ptr->data.number;

	bool found_var = false;
	double value = 0;

	if (TYPE == VAR)
	{
		found_var = determine_var_value(node_ptr->data.variable, preset_names, preset_names_num, &value);
		if (found_var) return value;
		
		found_var = determine_var_value(node_ptr->data.variable, name_table, name_table_size, &value);
		if (found_var) return value;
		else
		{
			printf_log_err("[from calculate_value] -> uninitialised variable\n");
			global_err_stat = error;
			return 0; 
		}
	}

	if (TYPE == OP)
	{
		double left_val = calculate_value(node_ptr->left);
		CHECK_ERR(0);
		double right_val = calculate_value(node_ptr->right);
		CHECK_ERR(0);

		for (int i = 0; i < op_count; i++)
		{
			if (node_ptr->data.operation == possible_ops[i].op)
			{
				return (*possible_ops[i].math_func)(left_val, right_val);
			}
		}
	}

	return -1;
}

#undef TYPE


bool determine_var_value(char var, name_record var_table[], int table_size, double* value)
{
	for (int i = 0; i < table_size; i++)
	{
		if (var == var_table[i].var)
		{
			*value =  var_table[i].value;
			return true;
		}
	}
	return false;
}


err_t process_calculating_partial_derivative(tree* tree_ptr)
{
	assert(tree_ptr != NULL);

	printf_debug_msg("process_calculating_partial_derivative: began process\n");

	printf_both("-> Enter differentiation variable name:\n");
	char diff_var = get_var_name();

	printf_both("-> Enter number of times to differentiate:\n");
	int diff_times = (int) request_number();

	tree* tree_ptr_arr[100] = {}; // TODO - make dynamic
	tree_ptr_arr[0] = tree_ptr;

	for (int i = 1; i <= diff_times; i++)
	{
		dump_text("Посчитаем производную %d-го порядка: \\\\ \n\n", i);
		
		tree_ptr_arr[i] = differentiate_tree(tree_ptr_arr[i-1], diff_var);
		CHECK_ERR(error);

		dump_text("По итогу получаем:\n\n");

		dump_start_of_differentiation(tree_ptr_arr[i-1]->root, diff_var);

		print_tree_dump(tree_ptr_arr[i], "Differentiated tree view (%d)\n", i);
		dump_intermediate_calculations(tree_ptr_arr[i]->root);

		optimize_equation(tree_ptr_arr[i]);

		dump_end_of_differentiation();

		dump_text("Всё, что недосократилось, сократите сами, РУЧКАМИ\n\n");
	}
	
	printf_debug_msg("process_calculating_partial_derivative: cleaning up tree_ptr_arr\n");

	for (int i = 1; i <= diff_times; i++) // TODO - delete in dtor in case user needs to count another derivative
	{
		if (tree_ptr_arr[i] != NULL) 
		{
			printf_debug_msg("cleaning %p\n", tree_ptr_arr[i]);
			destroy_tree(tree_ptr_arr[i]); 
		}
	}

	printf_debug_msg("process_calculating_partial_derivative: finished process\n");
	return ok;
}


tree* differentiate_tree(const tree* old_tree_ptr, char diff_var)
{
	assert(old_tree_ptr != NULL);
	printf_debug_msg("differentiate_tree: began process\n");

	tree* new_tree_ptr = tree_ctor();
	if (new_tree_ptr == NULL) return NULL;

	node* diffed_root_node = differentiate_node(new_tree_ptr, old_tree_ptr->root, diff_var);
	CHECK_ERR(NULL);

	new_tree_ptr->root = diffed_root_node;

	VERIFY_TREE(new_tree_ptr, NULL);
	printf_debug_msg("differentiate_tree: finished process\n");

	return new_tree_ptr;
}

node* differentiate_node(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert(tree_ptr != NULL);
	assert(current_node_ptr != NULL);

	printf_debug_msg("differentiate_node: began process\n");
	printf("node = %p type = %d (%d)\n", current_node_ptr, current_node_ptr->type, OP);

	node* diffed_node = NULL;

	switch(current_node_ptr->type)
	{
		case NUM:
			printf_debug_msg("differentiate_node: got number node\n")
			diffed_node = differentiate_number_node(tree_ptr, current_node_ptr);
			break;
		case VAR:
			printf_debug_msg("differentiate_node: got variable node\n")
			diffed_node  = differentiate_var_node(tree_ptr, current_node_ptr, diff_var);
			break;
		case OP:
			printf_debug_msg("differentiate_node: got operation node\n")
			diffed_node = differentiate_op_node(tree_ptr, current_node_ptr, diff_var);
			break;
		default:
			printf_log_err("[from differentiate_node] -> unknown node [%p] type\n", current_node_ptr);
			global_err_stat = error;
			return NULL;
	};

	CHECK_ERR(NULL);

	if (current_node_ptr->parent != NULL)
	{
		dump_start_of_differentiation(current_node_ptr, diff_var);
		dump_intermediate_calculations(diffed_node);
		dump_end_of_differentiation();
		insert_random_phrase();
	}

	printf_debug_msg("differentiate_node: finished process\n");

	return diffed_node;
}


const char* decode_operation_type_enum(diff_ops op)
{
    for (int i = 0; i < op_count; i++)
    {
		if (op == possible_ops[i].op)
			return possible_ops[i].name;
    }
	return "unknown";
}


double calculate_node(tree* tree_ptr, node* current_node) // TODO - work with sin/cos
{
	assert(tree_ptr != NULL);

	if (current_node == NULL) return 0;

	if (current_node->type == NUM)
		return current_node->data.number;

	if (current_node->type == VAR)
	{
		printf_log_err("[from calculate_node] -> cannot calculate variables\n");
		global_err_stat = error;
		return -1;
	}

	double res = 0;
	
	for (int i = 0; i < op_count; i++)
	{
		if (current_node->data.operation == possible_ops[i].op)
		{
			res = (*(possible_ops[i].math_func)) (calculate_node(tree_ptr, current_node->left), calculate_node(tree_ptr, current_node->right));
		}
	}

	return res;
}


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
		diffed_node_ptr->type = NUM;
		diffed_node_ptr->data.number = 0;
	}

	return diffed_node_ptr;
}


node* differentiate_op_node(tree* tree_ptr, node* current_node_ptr, char diff_var)
{
	assert(tree_ptr != NULL);
	assert(current_node_ptr != NULL);
	node* diffed_node_ptr = NULL;

	for (int i = 0; i < op_count; i++)
	{
		if (current_node_ptr->data.operation == possible_ops[i].op)
		{
			diffed_node_ptr = (*(possible_ops[i].diff_operation))(tree_ptr, current_node_ptr, diff_var);
		}
	}

	return diffed_node_ptr;
}


bool check_for_diff_var(node* current_node_ptr, char diff_var)
{
	if (current_node_ptr == NULL) return false;

	if (current_node_ptr->type == VAR)
	{
		if (current_node_ptr->data.variable == diff_var)
			return true;
	}
	return (check_for_diff_var(current_node_ptr->left,  diff_var) || 
			check_for_diff_var(current_node_ptr->right, diff_var));
}


int hash_var(char var)
{
	return var - 'a';
}

