#ifndef TREE_PROPERTIES_H
#define TREE_PROPERTIES_H

#include "differentiator_properties.h"

enum node_t
{
    UNKNOWN_TYPE,
    OP,
    VAR,
    NUM
};

union data_t 
{
    diff_ops operation;
    double number;
    char variable; // TODO - do char*
};

struct node
{
    node* parent;
    node_t type;
    data_t data;
    node* left;
    node* right;
};

struct tree
{
    node* root;
    size_t size;
    err_t err_stat;
    // char* text_buf; // TODO - will be required if variables are multi-symbol
};

#endif