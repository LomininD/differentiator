#include "reader.h"
#include "advanced_reader.h"
#include "differentiator_funcs.h"

// TODO - check errs

bool read_error;
node* get_general_tree(char** text_buf, size_t* tree_size);
node* get_expression(char** text_buf, size_t* tree_size);
node* get_term(char** text_buf, size_t* tree_size);
node* get_power(char** text_buf, size_t* tree_size);
node* get_parenthesis(char** text_buf, size_t* tree_size);
node* get_var(char** text_buf, size_t* tree_size);
node* get_number(char** text_buf, size_t* tree_size);

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

	if (num_len == 0) return NULL;
	
	skip_spaces(text_buf);
	node* new_node = create_and_initialise_node(NUM, (union data_t){.number = val}, NULL, NULL, NULL);
	if (is_neagtive) new_node->data.number *= -1;
	(*tree_size)++;
	return new_node;
}


node* get_var(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);

	char var_name = 0;

	if ('a' <= **text_buf && **text_buf <= 'z')
	{
		var_name = **text_buf;
    	name_table[hash_var(var_name)].var = var_name;
	}
	else
	{
		printf_log_err("[from get_var] -> cannot recognize variable name\n");
		read_error = true;
		return NULL;
	}

	(*text_buf)++;
	skip_spaces(text_buf);
	node* new_node = create_and_initialise_node(VAR, (union data_t){.variable = var_name}, NULL, NULL, NULL);
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
		assert_errs(NULL);

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
	{
		node* new_node = get_number(text_buf, tree_size);
		assert_errs(NULL);
		if (new_node == NULL) new_node = get_var(text_buf, tree_size);
		return new_node;
	}
}


node* get_expression(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);
	
	node* node_1 = get_term(text_buf, tree_size);
	assert_errs(NULL);

	skip_spaces(text_buf);

	while (**text_buf == '+' || **text_buf == '-')
	{
		char op = **text_buf;
		(*text_buf)++;
		skip_spaces(text_buf);

		node* node_2 = get_term(text_buf, tree_size);
		assert_errs(NULL);

		if (op == '+')
			node_1 = create_and_initialise_node(OP, (union data_t){.operation = ADD}, node_1, node_2, NULL);
		else
			node_1 = create_and_initialise_node(OP, (union data_t){.operation = SUB}, node_1, node_2, NULL);

		node_1->left->parent = node_1;
		node_1->right->parent = node_1;

		(*tree_size)++;
	}

	skip_spaces(text_buf);

	return node_1;
}


node* get_term(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);
	
	node* node_1 = get_power(text_buf, tree_size);
	assert_errs(NULL);

	skip_spaces(text_buf);

	while (**text_buf == '*' || **text_buf == '/')
	{
		char op = **text_buf;
		(*text_buf)++;
		skip_spaces(text_buf);

		node* node_2 = get_power(text_buf, tree_size);
		assert_errs(NULL);

		if (op == '*')
			node_1 = create_and_initialise_node(OP, (union data_t){.operation = MUL}, node_1, node_2, NULL);
		else
			node_1 = create_and_initialise_node(OP, (union data_t){.operation = DIV}, node_1, node_2, NULL);

		node_1->left->parent = node_1;
		node_1->right->parent = node_1;

		(*tree_size)++;
	}

	skip_spaces(text_buf);

	return node_1;
}


node* get_power(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);

	node* node_1 = get_parenthesis(text_buf, tree_size);
	assert_errs(NULL);

	skip_spaces(text_buf);

	while (**text_buf == '^')
	{
		(*text_buf)++;
		skip_spaces(text_buf);

		node* node_2 = get_parenthesis(text_buf, tree_size);
		assert_errs(NULL);

		node_1 = create_and_initialise_node(OP, (union data_t){.operation = POW}, node_1, node_2, NULL);
		
		node_1->left->parent = node_1;
		node_1->right->parent = node_1;

		(*tree_size)++;
	}

	skip_spaces(text_buf);

	return node_1;
}


node* get_general_tree(char** text_buf, size_t* tree_size)
{
	assert_errs(NULL);
	
	node* root_node = get_expression(text_buf, tree_size);
	assert_errs(NULL);

	skip_spaces(text_buf);

	if (**text_buf != '$')
	{
		printf_log_err("[from get_general_tree] -> no $ in the end of file");
	}

	(*text_buf)++;
	return root_node;
}