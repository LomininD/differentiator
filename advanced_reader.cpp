#include "reader.h"
#include "advanced_reader.h"

// TODO - check errs

bool read_error;
node* get_general_tree(char** text_buf, size_t* tree_size);
node* get_number(char** text_buf, size_t* tree_size);
node* get_parenthesis(char** text_buf, size_t* tree_size);
node* get_expression(char** text_buf, size_t* tree_size);

#define assert_errs(RET) if (read_error) return RET;

err_t advanced_read_formula(tree* tree_ptr)
{
    printf_debug_msg("read_formula: process started\n");

    FILE* load_ptr = fopen(FORMULA_FILE_NAME, "r");
    
    if (load_ptr == NULL)
    {
        printf_log_err("[from read_formula] -> could not open file %s (file not found)", FORMULA_FILE_NAME);
		return error;
    }

    char* text_buf = NULL;
    err_t file_is_read = read_text_buf(load_ptr, &text_buf);
    if (file_is_read != ok) return error;
	tree_ptr->text_buf = text_buf;
    size_t node_count = 0;
    
	tree_ptr->root = get_general_tree(&text_buf, &node_count);
	tree_ptr->size = node_count;

    fclose(load_ptr);

	VERIFY_TREE(tree_ptr, error);

    printf_debug_msg("read_formula: process finished\n");
	print_tree_dump(tree_ptr, "Read formula tree view\n");

    return ok;
}


node* get_number(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);
	
	int val = 0;
	int num_len = 0;
	bool is_neagtive = false;
	puts(*text_buf);

	while(('0' <= **text_buf && **text_buf <= '9') || **text_buf == '-')
	{
		if (**text_buf == '-')
		{
			if (!is_neagtive && num_len == 0)
			{
				is_neagtive = true;
				printf("negative\n");
				(*text_buf)++;
				continue;
			}
			else
			{
				printf_log_err("[from get_number] -> got wrong minus in number\n");
				read_error = true;
				return NULL;
			}
		}
		val = val * 10 + (**text_buf - '0');
		(*text_buf)++;
		num_len++;
	} 

	if (num_len == 0)
	{
		printf_log_err("[from advanced_reader] -> got corrupted number\n");
		read_error = true;
		return NULL;
	}
	
	skip_spaces(text_buf);
	node* new_node = create_and_initialise_node(NUM, (union data_t){.number = val}, NULL, NULL, NULL);
	if (is_neagtive) new_node->data.number *= -1;
	(*tree_size)++;
	return new_node;
}


node* get_parenthesis(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);
	
	if (**text_buf == '(')
	{
		(*text_buf)++;
		node* new_node = get_expression(text_buf, tree_size);

		if (**text_buf != ')')
		{
			printf_log_err("[from get_parenthesis] -> no closing parenthesis\n");
			read_error = true;
			return NULL;
		}

		(*text_buf)++;
		skip_spaces(text_buf);
		return new_node;
	}
	else
		return get_number(text_buf, tree_size);
}


node* get_expression(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);
	
	node* node_1 = get_parenthesis(text_buf, tree_size);
	
	node* new_node = NULL;

	skip_spaces(text_buf);

	while (**text_buf == '+' || **text_buf == '-')
	{
		char op = **text_buf;
		(*text_buf)++;
		skip_spaces(text_buf);

		node* node_2 = get_parenthesis(text_buf, tree_size);

		if (op == '+')
			new_node = create_and_initialise_node(OP, (union data_t){.operation = ADD}, node_1, node_2, NULL);
		else
			new_node = create_and_initialise_node(OP, (union data_t){.operation = SUB}, node_1, node_2, NULL);

		node_1->parent = new_node;
		node_2->parent = new_node;

		(*tree_size)++;
		node_1 = new_node;
	}

	skip_spaces(text_buf);

	return new_node;
}


node* get_general_tree(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);
	
	node* root_node = get_expression(text_buf, tree_size);

	skip_spaces(text_buf);

	if (**text_buf != '$')
	{
		printf_log_err("[from get_general_tree] -> no $ in the end of file");
	}

	(*text_buf)++;
	return root_node;
}