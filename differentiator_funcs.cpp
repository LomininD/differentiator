#include "differentiator_funcs.h"
#include "dmath.h"
#include <stdlib.h>
#include <assert.h>


// static void write_node(FILE* save_ptr, const tree* tree, const node* current_node);
// static err_t process_saving(const tree* tree);
// static err_t process_loading(tree* tree);
static tree* differentiate_tree(const tree* old_tree_ptr, char diff_var);


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


err_t process_calculating_partial_derivative(tree* tree_ptr)
{
	assert(tree_ptr != NULL);

	printf_debug_msg("process_calculating_partial_derivative: began process\n");

	printf_both("-> Enter differentiation variable name:\n");
	char diff_var = get_var_name();

	printf_both("-> Enter number of times to differentiate:\n");
	int diff_times = get_number();

	tree* tree_ptr_arr[100] = {}; // TODO - make dynamic
	tree_ptr_arr[0] = tree_ptr;

	for (int i = 1; i <= diff_times; i++)
	{
		tree_ptr_arr[i] = differentiate_tree(tree_ptr_arr[i-1], diff_var);
		CHECK_ERR(error);

		print_tree_dump(tree_ptr_arr[i], "Differentiated tree view (%d)\n", i);

		optimize_equation(tree_ptr_arr[i]);
		// dump_to_tex
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

// err_t process_saving(const tree* tree)
// {
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "\n");
//     printf_log_msg(debug_mode, "save_database: started writing in file\n");
// 
//     FILE* save_ptr = fopen(SAVE_FILE_NAME, "w");
//     write_node(save_ptr, tree, tree->root);
//     fclose(save_ptr);
//     say_message(debug_mode, "-> Database saved successfully\n", "Database saved successfully", NULL);
// 
//     printf_log_msg(debug_mode, "save_database: done writing in file\n\n");
//     return ok;
// }
// 
// #define FPRINT(...) fprintf(save_ptr, __VA_ARGS__)
// 
// void write_node(FILE* save_ptr, const tree* tree, const node* current_node)
// {
//     printf_log_msg(tree->debug_mode, "writing [%p]\n", current_node);
// 
//     if (current_node == NULL) { FPRINT("nil "); return; }
//     
//     FPRINT("( ");
//     FPRINT("\"%s\" ", current_node->string);
//     write_node(save_ptr, tree, current_node->yes_branch);
//     write_node(save_ptr, tree, current_node->no_branch);
//     FPRINT(") ");
// }
// 
// #undef FPRINT
// 
// err_t load_database(tree* tree)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "load_database: process began\n");
// 
//     say_message(debug_mode, "-> Loading saved version of database will overwrite current version of it.\n",\
//                      "Loading saved version of database will overwrite current version of it", NULL);
//     say_message(debug_mode, "-> Load anyway? ([y]es / [n]o)\n", "Load anyway", NULL);
// 
//     ans_t ans = get_answer(debug_mode);
// 
//     switch (ans)
//     {
//         case yes:
//             process_loading(tree);
//             break;
//         case no:
//             break;
//         default:
//             return error;
//     };
// 
//     VERIFY_TREE(error);
// 
//     print_tree_dump(tree, "Displaying freshly read tree", NULL);
//     printf_log_msg(debug_mode, "load_database: process finished\n");
//     return ok;
// }








