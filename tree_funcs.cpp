#include "tree_funcs.h"
#include <stdlib.h>
#include <assert.h>

extern err_t global_err_stat;

tree* tree_ctor()
{
    tree* tree_ptr = (tree*) calloc(1, sizeof(tree));

    if (tree_ptr == NULL)
    {
        printf_log_err("[from tree_ctor] -> could not allocate memory for tree\n");
        global_err_stat = error;
        return NULL;
    }

    printf_debug_msg("tree_ctor: began initialising tree [%p]\n", tree_ptr);

    VERIFY_TREE(tree_ptr, NULL);

    printf_debug_msg("tree_ctor: finished initialising tree [%p]\n", tree_ptr);
    return tree_ptr;
}


// node* copy_node(node* original_node_ptr)
// {
//     printf_debug_msg("copy_node: began process\n");
// 
//     node* copied_node_ptr = create_and_initialise_node(original_node_ptr->type, original_node_ptr->data, \
//                                                                                     NULL, NULL, NULL);
// 
//     printf_debug_msg("copy_node: finished process\n");
//     return copied_node_ptr;
// }


node* create_node()
{
    printf_debug_msg("create_node: began creating node\n");

    node* new_node_ptr = (node*) calloc(1, sizeof(node));

    assert(new_node_ptr != NULL);

    printf_debug_msg("create_node: done creating node [%p]\n", new_node_ptr);
    return new_node_ptr;
}


#define node_data new_node_ptr->data

node* create_and_initialise_node(node_t type, union data_t data, node* left, node* right, node* parent)
{
    printf_debug_msg("create_and_initialise_node: began process\n");

    node* new_node_ptr = create_node();

    new_node_ptr->type = type;

    switch(type)
    {
        case OP:
            node_data.operation = data.operation;
            break;
        case VAR:
            data.variable = data.variable;
            break;
        case NUM:
            node_data.number = data.number;
            break;
        default:
            printf_log_err("[from copy_node] -> could not recognize type of node\n");
    };

    new_node_ptr->left = left;
    new_node_ptr->right = right;
    new_node_ptr->parent = parent;

    printf_debug_msg("create_and_initialise_node: finished process\n");

    return new_node_ptr;
}

#undef node_data


void destroy_tree(tree* tree_ptr)
{
    assert(tree_ptr != NULL);

    printf_debug_msg("destroy_tree: began cutting down tree\n");

    destroy_node(tree_ptr->root);

    tree_ptr->root = NULL;
    free(tree_ptr->text_buf);
    free(tree_ptr);

    printf_debug_msg("destroy_tree: finished cutting down tree\n");
}


void destroy_node(node* node)
{
    if (node == NULL) return;
    printf_debug_msg("deleting %p\n", node);

    if (node->left != NULL)
    {
        destroy_node(node->left);
    }
    if (node->right)
    {
        destroy_node(node->right); // TODO - if we have char* consider freeing it
    }

    free(node);
}



