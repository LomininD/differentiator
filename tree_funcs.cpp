#include "tree_funcs.h"
#include <stdio.h>
#include <assert.h>


err_t tree_ctor(tree* tree)
{
    if (tree == NULL)
    {
        printf_log_err("[from tree_ctor] -> tree not found\n");
        return error;
    }

    printf_debug_msg("tree_ctor: began initialising tree [%p]", tree);

    VERIFY_TREE(error);

    printf_debug_msg("tree_ctor: finished initialising tree [%p]", tree);
    return ok;
}


node* create_node()
{
    printf_debug_msg("create_node: began creating node\n");

    node* new_node = (node*) calloc(1, sizeof(node));

    printf_debug_msg("create_node: done creating node [%p]\n", new_node);
    return new_node;
}


void destroy_tree(tree* tree)
{
    assert(tree != NULL);

    printf_debug_msg("destroy_tree: began cutting down tree\n");

    destroy_node(tree->root);

    tree->root = NULL;
    //free(tree->text_buf);

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



