#include "reader.h"
#include "tree_funcs.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static node* read_node(char** current_pos, size_t* node_count, err_t* read_error);
static err_t parse_node_data(node* new_node, char* string);
static void skip_spaces(char** current_pos);

static err_t process_operation(node* new_node, diff_ops op);
static err_t process_number(node* new_node, double number, const char* end_of_str);
static err_t process_variable(node* new_node, char* string);

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
    size_t bytes_in_file = (size_t) file_info.st_size / sizeof(char);

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

	err_t is_read = ok;
    node* root_node = read_node(&text_buf, &node_count, &is_read);
	if (is_read != ok) return error;
    tree->root = root_node;
    tree->size = node_count;

    fclose(load_ptr);

	VERIFY_TREE(error);

    printf_debug_msg("read_formula: process finished\n");

	print_tree_dump(tree, "Read formula tree view");

    return ok;
}


node* read_node(char** current_pos, size_t* node_count, err_t* read_error)
{
    skip_spaces(current_pos);
    if (**current_pos == '(')
    {
        printf_debug_msg("read_node: got '(', began reading node\n");
        printf_debug_msg("%s", *current_pos);

        node* new_node = create_node();
        if (new_node == NULL) { *read_error = error; return NULL; }
        (*node_count)++;

        (*current_pos)++; skip_spaces(current_pos);

        printf_debug_msg("read_node: started reading node string\n");
        printf_debug_msg("%s", *current_pos);

        int name_len = 0;
        sscanf(*current_pos, "%*s%n", &name_len);
        *strchr(*current_pos, ' ') = '\0';

        printf_debug_msg("read_node: done reading node string (%s)\n", *current_pos);

		err_t parsed =  parse_node_data(new_node, *current_pos);
		if (parsed != ok) { *read_error = error; return NULL; }

        *current_pos += name_len + 1;

        printf_debug_msg("read_node: node name assigned\n");
        printf_debug_msg("%s", *current_pos); skip_spaces(current_pos);

        printf_debug_msg("read_node: began reading left subtree\n");
        printf_debug_msg("%s", *current_pos);

        node* left_node = read_node(current_pos, node_count, read_error); skip_spaces(current_pos);
		if (*read_error != ok) return NULL;
        new_node->left = left_node;
		if (left_node != NULL) left_node->parent = new_node;

        printf_debug_msg("read_node: began reading right subtree\n"); 
        printf_debug_msg("%s", *current_pos);

        node* right_node = read_node(current_pos, node_count, read_error); skip_spaces(current_pos);
		if (*read_error != ok) return NULL;
        new_node->right = right_node;
		if (right_node != NULL) right_node->parent = new_node;

        if (**current_pos != ')')
        {
            printf_log_err("[from read_node] -> corrupted formula file (no ')')\n");
			*read_error = error;
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
        printf_log_err("[from read_node] -> corrupted formula file (no '(' or nil)\n");
		*read_error = error;
        return NULL;
    }
}


err_t parse_node_data(node* new_node, char* string) // TODO - split into functions, move to another file
{
	assert(new_node != NULL);
	assert (string != NULL);

	printf_debug_msg("parse_node_data: process started\n");
    err_t processed = ok;

	if (strcmp(string, "+") == 0) // TODO - remove copy pasting
		processed = process_operation(new_node, ADD);
	else if (strcmp(string, "-") == 0)
        processed = process_operation(new_node, SUB);

	char* end_of_str = string;
	double number = strtod(string, &end_of_str);

	if (*end_of_str != *string)
		processed = process_number(new_node, number, end_of_str);
    else
        processed = process_variable(new_node, string);
    
    if (processed != ok) return error;

	return ok;
}


err_t process_operation(node* new_node, diff_ops op)
{
    new_node->type = OP;
    new_node->data.operation = op;

    printf_debug_msg("process_operation: recognized operation %s\n", decode_operation_type_enum(op));

    return ok;
}


err_t process_number(node* new_node, double number, const char* end_of_str)
{
    if (number == HUGE_VAL)
		{
			printf_log_err("[from parse_node_data] -> number is too big\n");
			return error;
		}

		printf("%c\n", *(end_of_str + 1));

		if (*end_of_str != '\0')
		{
			printf_log_err("[from parse_node_data] -> corrupted number\n");
			return error;
		}

		printf_debug_msg("parse_node_data: recognized number %lf\n", number);

		new_node->type = NUM;
		new_node->data.number = number;
		return ok;
}


err_t process_variable(node* new_node, char* string)
{
    if (strlen(string) != 1)
	{
		printf_log_err("[from process_variable] -> variable name is too big\n");
		return error;
	}

	new_node->type = VAR;
	new_node->data.variable = *string;

	printf_debug_msg("process_variable: recognized variable name %s\n", string);
    return ok;
}


void skip_spaces(char** current_pos)
{
    while (**current_pos == ' ')
    {
        (*current_pos)++;
    }
}
