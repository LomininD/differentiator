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


#define create_copy(data)                                                   \
            create_and_initialise_node(original_node_ptr->type, data,       \
            copy_node(tree_ptr, original_node_ptr->left),                   \
            copy_node(tree_ptr, original_node_ptr->right), NULL);           \

node* copy_node(tree* tree_ptr, node* original_node_ptr)
{
    printf_debug_msg("copy_node: began process\n");

    if (original_node_ptr == NULL) return NULL;

    node* copied_node_ptr = NULL;

    switch(original_node_ptr->type)
    {
        case OP:
            copied_node_ptr = create_copy((union data_t){.operation = original_node_ptr->data.operation});
            break;
        case NUM:
            copied_node_ptr = create_copy((union data_t){.number = original_node_ptr->data.number});
            break;
        case VAR:
            copied_node_ptr = create_copy((union data_t){.variable = original_node_ptr->data.variable});
            break;
        default:
            return NULL;
    };

    tree_ptr->size++;
    printf_debug_msg("copy_node: created copy %p\n", copied_node_ptr);

    printf_debug_msg("copy_node: finished process\n");
    return copied_node_ptr;
}

#undef create_copy


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
            node_data.variable = data.variable;
            break;
        case NUM:
            node_data.number = data.number;
            break;
        default:
            printf_log_err("[from copy_node] -> could not recognize type of node\n");
    };

    if (left != NULL) left->parent = new_node_ptr;
    if (right != NULL) right->parent = new_node_ptr;

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


size_t destroy_node(node* node)
{
    if (node == NULL) return 0;

    size_t nodes_destroyed = 0;

    if (node->left != NULL)
    {
        nodes_destroyed += destroy_node(node->left);
    }
    if (node->right != NULL)
    {
        nodes_destroyed += destroy_node(node->right); // TODO - if we have char* consider freeing it
    }

    printf_debug_msg("deleting %p\n", node);

    free(node);

    nodes_destroyed ++;

    return nodes_destroyed;
}



