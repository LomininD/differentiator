#include "reader.h"
#include "advanced_reader.h"
#include "differentiator_funcs.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "dmath.h"

// TODO - check errs
// TODO - add instructions for binary funcs


const int max_func_name_len = 10;

bool read_error;
node* get_general_tree(char** text_buf, size_t* tree_size);
node* get_expression(char** text_buf, size_t* tree_size);
node* get_term(char** text_buf, size_t* tree_size);
node* get_power(char** text_buf, size_t* tree_size);
node* get_func(char** text_buf, size_t* tree_size, char* func_name);
node* get_parenthesis(char** text_buf, size_t* tree_size);
node* get_var(size_t* tree_size, char* word);
char* get_word(char** text_buf);
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
	
	double val = 0;
	int num_len = 0;
	bool is_negative = false;
	bool has_floating_point = false;
	int after_floating_point = 0;

	//puts(*text_buf);

	while(('0' <= **text_buf && **text_buf <= '9') || **text_buf == '-' || ** text_buf == '.')
	{
		if (**text_buf == '-')
		{
			if (!is_negative)
			{
				if (num_len == 0)
				{
					is_negative = true;
					(*text_buf)++;
					continue;
				}
				else break;
			}
			else
			{
				printf_log_err("[from get_number] -> got wrong minus in number\n");
				read_error = true;
				return NULL;
			}
		}

		if (**text_buf == '.')
		{
			if (!has_floating_point && num_len > 0) 
			{
				has_floating_point = true;
				(*text_buf)++;
			}
			else 
			{
				printf_log_err("[from get_number] -> got wrong floating point in number\n");
				read_error = true;
				return NULL;
			}
		}

		if (has_floating_point)
		{
			after_floating_point++;
			val = val + (double) (**text_buf - '0') / pow(10.0, after_floating_point);
			printf("%lg\n", val);
		}
		else 
		{
			val = val * 10.0 + (double) (**text_buf - '0');
			printf("%lg\n", val);
		}

		(*text_buf)++; 
		num_len++;
	} 

	if (num_len == 0) return NULL;
	
	skip_spaces(text_buf);
	node* new_node = create_and_initialise_node(NUM, (union data_t){.number = val}, NULL, NULL, NULL);
	if (is_negative) new_node->data.number *= -1;
	(*tree_size)++;
	return new_node;
}


node* get_var(size_t* tree_size, char* word)
{
	assert_errs(NULL);
	assert(word != NULL);

	char var_name = 0;

	if (word[1] == '\0')
	{
    	var_name = word[0];
		if (!is_preset(var_name)) name_table[hash_var(var_name)].var = var_name;
		free(word);
	}
	else
	{
		printf_log_err("[from get_var] -> variable name is too big\n");
		free(word);
		read_error = true;
		return NULL;
	}

	node* new_node = create_and_initialise_node(VAR, (union data_t){.variable = var_name}, NULL, NULL, NULL);
	(*tree_size)++;
	return new_node;
}


char* get_word(char** text_buf)
{
	assert_errs(NULL);

	char word[max_func_name_len + 1] = {};
	int word_len = 0;

	while ('a' <= **text_buf && **text_buf <= 'z' && word_len < max_func_name_len + 1)
	{
		word[word_len] = **text_buf;
		word_len++;
		(*text_buf)++;
	}

	if (word_len == max_func_name_len + 1)
	{
		printf_log_err("[from get_word] -> word is too long\n");
		read_error = true;
		return NULL;
	} 

	if (word_len == 0) 
	{
		printf_log_err("[from get_word] -> cannot recognize the word\n");
		read_error = true;
		return NULL;
	}

	skip_spaces(text_buf);
	//puts(*text_buf);
	//puts(word);
	return strdup(word);
}


node* get_func(char** text_buf, size_t* tree_size, char* func_name)
{
	assert_errs(NULL);

	assert_errs(NULL);
	if (func_name == NULL) return NULL;

	bool found_func = false;
	node* new_node = NULL;

	for (int i = 0; i < op_count; i++)
	{
		//printf("%s %s\n", func_name, possible_ops[i].name);
		if (strcmp(func_name, possible_ops[i].name) == 0)
		{
			found_func = 1;

			free(func_name);

			skip_spaces(text_buf);

			node* exp_node = NULL;

			if (**text_buf == '(')
			{
				(*text_buf)++;
				skip_spaces(text_buf);
				exp_node = get_expression(text_buf, tree_size);
				assert_errs(NULL);
			}
			else
			{
				printf_log_err("[from get_func] -> no opening parenthesis\n");
				read_error = true;
				return NULL;
			}

			skip_spaces(text_buf);

			if (**text_buf != ')')
			{
				printf_log_err("[from get_func] -> no closing parenthesis\n");
				read_error = true;
				return NULL;
			}

			(*text_buf)++;
			skip_spaces(text_buf);

			new_node = create_and_initialise_node(OP, (union data_t){.operation = possible_ops[i].op}, NULL, exp_node, NULL);
			exp_node->parent = new_node;
			(*tree_size)++;
			break;
		}
	}

	if (!found_func) return NULL;

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

		if (new_node == NULL) 
		{
			char* word = get_word(text_buf);
			assert_errs(NULL);

			new_node = get_func(text_buf, tree_size, word);
			assert_errs(NULL);

			if (new_node == NULL) new_node = get_var(tree_size, word);

		}

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
		printf_log_err("[from get_general_tree] -> no $ in the end of file\n");
	}

	(*text_buf)++;
	return root_node;
}