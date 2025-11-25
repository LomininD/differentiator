#include "differentiator_funcs.h"
#include <stdlib.h>
#include <assert.h>


// static void write_node(FILE* save_ptr, const tree* tree, const node* current_node);
// static err_t process_saving(const tree* tree);
// static err_t process_loading(tree* tree);
static tree* differentiate_tree(tree* tree_ptr, char diff_var);
node* differentiate_node(tree* tree_ptr, node* current_node_ptr);


void print_menu()
{
	printf_both("-> Choose a command to execute:\n");
	printf("- [c]ount\n");
	printf("- [s]implify\n");
	printf("- [d]erivative\n");
	printf("- [p]artial derivative\n");
	printf("- [g]raph\n");
	printf("- [t]aylor polynomial\n");
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
	}

	// dump_to_tex
	
	print_tree_dump(tree_ptr_arr[diff_times], "Differentiated tree view");

	printf_debug_msg("process_calculating_partial_derivative: cleaning up tree_ptr_arr\n");

	for (int i = 1; i <= diff_times; i++) // TODO - delete in dtor in case user needs to count another derivative
	{
		if (tree_ptr_arr[i] != NULL) free(tree_ptr_arr[i]);
	}

	printf_debug_msg("process_calculating_partial_derivative: finished process\n");
	return ok;
}


tree* differentiate_tree(tree* tree_ptr, char diff_var)
{
	assert(tree_ptr != NULL);
	printf_debug_msg("differentiate_tree: began process\n");

	tree* new_tree_ptr = tree_ctor();
	if (new_tree_ptr == NULL) return NULL;

	node* diffed_root_node = differentiate_node(tree_ptr, tree_ptr->root);
	CHECK_ERR(NULL);

	new_tree_ptr->size += 1;

	VERIFY_TREE(tree_ptr, NULL);
	printf_debug_msg("differentiate_tree: finished process\n");

	return new_tree_ptr;
}

node* differentiate_node(tree* tree_ptr, node* current_node_ptr)
{
	assert(tree_ptr != NULL);
	assert(current_node_ptr != NULL);

	node* diffed_node = NULL;

	switch(current_node_ptr->type)
	{
		case NUM:
			// differentiate_number_node()
			break;
		case VAR:
			// differentiate_var_node()
			break;
		case OP:
			// differentiate_op_node()
			break;
		default:
			printf_log_err("[from differentiate_node] -> unknown node [%p] type\n", current_node_ptr);
			global_err_stat = error;
			return NULL;
	};

	CHECK_ERR(NULL);
	return diffed_node;
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








