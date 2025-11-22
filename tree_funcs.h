#ifndef TREE_FUNCS_H
#define TREE_FUNCS_H

#include "tree_properties.h"
#include "tree_dump.h" 


node* create_node();
err_t tree_ctor(tree* tree);
void destroy_tree(tree* tree);
void destroy_node(node* node);


#ifdef TREE_VERIFICATION

#define VERIFY_TREE(RET) if (process_tree_verification(tree) != ok)  return RET;

#else

# define VERIFY_TREE(RET) if (tree == NULL) return RET;

#endif

#define DISPLAY_TREE() if (debug_mode == on) generate_dump_image(tree);

#endif