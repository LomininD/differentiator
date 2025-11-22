#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "differentiator_funcs.h"
#include "debug_lib/debug.h"
#include "tree_dump.h"

static err_t parse_node_data(node* new_node, char* string);


// static void write_node(FILE* save_ptr, const tree* tree, const node* current_node);
// static err_t process_saving(const tree* tree);
// static err_t process_loading(tree* tree);
static node* read_node(char** current_pos, size_t* node_count);
static void skip_spaces(char** current_pos);


// err_t save_database(const tree* tree)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "save_database: began saving database\n");
// 
//     say_message(debug_mode, "-> Saving current version of database will overwrite saved version of it.\n", \
//                             "Saving current version of database will overwrite saved version of it", NULL);
//     say_message(debug_mode, "-> Save anyway? ([y]es / [n]o)\n", "Save anyway", NULL);
// 
//     ans_t ans = get_answer(debug_mode);
//     err_t saved = ok; // FIXME - how to remove
// 
//     switch (ans)
//     {
//         case yes:
//             saved = process_saving(tree);
//             if (saved != ok) return saved;
//             break;
//         case no:
//             break;
//         default:
//             return error;
//     };
// 
//     printf_log_msg(debug_mode, "save_database: finished process\n");
// 
//     return ok;
// }
// 
// 
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


err_t read_formula(tree* tree)
{
    printf_debug_msg("read_formula: process started\n");

    FILE* load_ptr = fopen(FORMULA_FILE_NAME, "r");
    
    if (load_ptr == NULL)
    {
        printf_log_err("[from read_formula] -> could not open file %s (file not found)", FORMULA_FILE_NAME);
		return error;
    }

    struct stat file_info = {};
    fstat(fileno(load_ptr), &file_info);
    size_t bytes_in_file = file_info.st_size / sizeof(char);

    char* text_buf = (char*) calloc (bytes_in_file + 1, sizeof(char));

    if (text_buf == NULL) 
    {
        printf_log_err("[from read_formula] -> not enough memory to load formula\n");
        return error;
    }

    size_t bytes_read = fread(text_buf, sizeof(char), bytes_in_file, load_ptr);

    printf_debug_msg("read_formula: bytes in file: %zu\n", bytes_in_file);
    printf_debug_msg("read_formula: bytes read:    %zu\n", bytes_read);

	assert(bytes_in_file == bytes_read);

    *(text_buf + bytes_in_file) = '\0';

	tree->text_buf = text_buf;
    size_t node_count = 0;

    node* root_node = read_node(&text_buf, &node_count);
    tree->root = root_node;
    tree->size = node_count;

    fclose(load_ptr);

	VERIFY_TREE(error);

    printf_debug_msg("read_formula: process finished\n");

    return ok;
}


node* read_node(char** current_pos, size_t* node_count) // TODO - simplify
{
    skip_spaces(current_pos);
    if (**current_pos == '(')
    {
        printf_debug_msg("read_node: got '(', began reading node\n");
        printf_debug_msg("%s", *current_pos);

        node* new_node = create_node();
        if (new_node == NULL) return NULL;
        (*node_count)++;

        (*current_pos)++;
        skip_spaces(current_pos);

        printf_debug_msg("read_node: started reading node name\n");
        printf_debug_msg("%s", *current_pos);

        int name_len = 0;
        sscanf(*current_pos, "%*s%n", &name_len);

        printf_debug_msg("read_node: done reading node name\n");

        *strchr(*current_pos, ' ') = '\0';

		printf_debug_msg("read_node: read node string: %s\n", *current_pos);

		err_t parsed =  parse_node_data(new_node, *current_pos);
		if (parsed != ok) return NULL;

        *current_pos += name_len + 1;

        printf_debug_msg("read_node: node name assigned\n");
        printf_debug_msg("%s", *current_pos);

        skip_spaces(current_pos);

        printf_debug_msg("read_node: began reading left subtree\n");
        printf_debug_msg("%s", *current_pos);

        node* left_node = read_node(current_pos, node_count);
        new_node->left = left_node;
		if (left_node != NULL) left_node->parent = new_node;

        skip_spaces(current_pos);

        printf_debug_msg("read_node: began reading right subtree\n");
        printf_debug_msg("%s", *current_pos);

        node* right_node = read_node(current_pos, node_count);
        new_node->right = right_node;
		if (right_node != NULL) right_node->parent = new_node;

        skip_spaces(current_pos);

        if (**current_pos != ')')
        {
            printf_log_err("[from read_node] -> corrupted save file (no ')')\n");
            return NULL;
        }

        (*current_pos)++;

        printf_debug_msg("read_node: node is read\n");
        printf_debug_msg("%s", *current_pos);
        
        return new_node;
    }

    else if (**current_pos == 'n' && *(*current_pos + 1) == 'i' && *(*current_pos + 2) == 'l')                   
    {
        *current_pos += 3;

        printf_debug_msg("read_node: got nil\n");
        printf_debug_msg("%s", *current_pos);

        return NULL;
    }
    else
    {
        printf_log_err("[from read_node] -> corrupted save file (no '(' or nil)\n");
        return NULL;
    }
}


err_t parse_node_data(node* new_node, char* string) // TODO - split into functions, move to another file
{
	assert(new_node != NULL);
	assert (string != NULL);

	printf_debug_msg("parse_node_data: process started\n");

	char* end_of_str = string;

	if (strcmp(string, "+") == 0) // TODO - remove copy pasting
	{
		new_node->type = OP;
		new_node->data.operation = ADD;

		printf_debug_msg("parse_node_data: recognized operation %s\n", decode_operation_type_enum(ADD));

		return ok;
	}
	else if (strcmp(string, "-") == 0)
	{
		new_node->type = OP;
		new_node->data.operation = SUB;

		printf_debug_msg("parse_node_data: recognized operation %s\n", decode_operation_type_enum(SUB));

		return ok;
	}

	double number = strtod(string, &string);

	if (*end_of_str != *string)
	{
		if (number == HUGE_VAL)
		{
			printf_log_err("[from parse_node_data] -> number is too big\n");
			return error;
		}

		if (*(end_of_str + 1) != '\0')
		{
			printf_log_err("[from parse_node_data] -> corrupted number\n");
			return error;
		}

		printf_debug_msg("parse_node_data: recognized number %lf\n", number);

		new_node->type = NUM;
		new_node->data.number = number;
		return ok;
	}

	if (strlen(string) != 1)
	{
		printf_log_err("[from parse_node_data] -> variable name is too big\n");
		return error;
	}

	new_node->type = VAR;
	new_node->data.variable = *string;

	printf_debug_msg("parse_node_data: recognized variable name %s\n", string);

	return ok;
}


void skip_spaces(char** current_pos)
{
    while (**current_pos == ' ')
    {
        (*current_pos)++;
    }
}


// err_t check_prediction(tree* tree, node* current_node)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "check_prediction: began checking guessed object\n");
// 
//     say_message(debug_mode, "-> This is %s, am I right? ([y]es / [n]o)\n", "This is %s, am I right",  current_node->string);
//     ans_t ans = get_answer(debug_mode);
//     err_t requested = ok; // FIXME - not supposed to be here
// 
//     switch (ans)
//     {
//         case yes:
//             say_message(debug_mode, "-> Yeah, I am always right.\n", "Yeah, I am always right", NULL);
//             return ok;
//         case no:
//             requested = request_new_nodes(tree, current_node);
//             if (requested != ok) return requested;
//             return ok;
//         default:
//             return error;
//     };
// }
// 
// 
// // err_t give_definition(tree* tree, char* obj_name)
// // {
// //     VERIFY_TREE(error);
// //     assert(obj_name != NULL);
// // }
// // 
// // 
// // node* search_obj(tree* tree, node* current_node, const char* name)
// // {
// //     if (current_node == NULL)
// //         return NULL;
// //     if (strcmp(name, current_node->string) == 0)
// //         return current_node;
// //     
// //     node* found_node = search_obj(tree, current_node->yes_branch, name);
// //     if (found_node == NULL)
// //         found_node = search_obj(tree, current_node->no_branch, name);
// //         
// // }
// 
// 
// err_t request_tree_beginning(tree* tree)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "request_new_node: began requesting tree beginning\n");
// 
//     say_message(debug_mode, "-> Hm... It seems that there is no data, where can it be?\n", \
//                         "Hm... It seems that there is no data, where can it be", NULL);
//     say_message(debug_mode, "-> Lets create and set up database now.\n", \
//                     "Lets create and set up data base now", NULL);
// 
//     say_message(debug_mode, "-> Enter first object name, please\n", "Enter first object name, please", NULL); // FIXME - copy pasting
//     char* first_obj_name = request_string(debug_mode);
//     if (first_obj_name == NULL) return error;
//     
//     say_message(debug_mode, "-> Enter second object name, please:\n", "Enter second object name, please", NULL);
//     char* second_obj_name = request_string(debug_mode);
//     if (second_obj_name == NULL) return error;
// 
//     node* yes_node = create_node(debug_mode);
//     node* no_node  = create_node(debug_mode);
// 
//     yes_node->string = first_obj_name;
//     no_node->string  = second_obj_name;
// 
//     yes_node->freshly_created = true;
//     no_node->freshly_created  = true;
// 
//     say_message(debug_mode, "-> How does %s differ from %s?\n", "How does %s differ from %s", first_obj_name, second_obj_name);
//     char* diff_str = request_string(debug_mode);
//     
//     node* root_node = create_node(debug_mode);
//     root_node->string = diff_str;
//     root_node->yes_branch = yes_node;
//     root_node->no_branch = no_node;
//     root_node->freshly_created = true;
// 
//     tree->root = root_node;
//     tree->size = 3;
//     
//     VERIFY_TREE(error);
// 
//     printf_log_msg(debug_mode, "request_new_node: tree beginning requested\n");
// 
//     DISPLAY_TREE();
// 
//     return ok;
// }
// 
// 
// err_t request_new_nodes(tree* tree, node* parent_node)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "request_new_node: began requesting new nodes\n");
// 
//     say_message(debug_mode, "-> What is it?\n", "What is it", NULL);
// 
//     char* obj_name  = request_string(debug_mode);
// 
//     if (obj_name == NULL)
//         return error;
// 
//     say_message(debug_mode, "-> How does it differ from %s?\n", "How does it differ from %s", parent_node->string);
// 
//     char* diff_string  = request_string(debug_mode);
// 
//     if (diff_string == NULL)
//         return error;
// 
//     node* new_left_node = create_node(debug_mode);
//     node* new_right_node = create_node(debug_mode);
// 
//     new_right_node->string = parent_node->string;
//     new_left_node->string = obj_name;
//     parent_node->string = diff_string;
//     parent_node->yes_branch = new_left_node;
//     parent_node->no_branch = new_right_node;
//     parent_node->freshly_created = true;
//     new_left_node->freshly_created = true;
// 
//     tree->size += 2;

//
// 
//     VERIFY_TREE(error);
//     
//     printf_log_msg(debug_mode, "request_new_node: new nodes are requested\n");
// 
//     DISPLAY_TREE()
//     return ok;
// }
