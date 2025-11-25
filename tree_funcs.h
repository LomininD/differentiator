#ifndef TREE_FUNCS_H
#define TREE_FUNCS_H

#include "tree_properties.h"
#include "tree_dump.h" 


node* create_node();
tree* tree_ctor();
void destroy_tree(tree* tree);
void destroy_node(node* node);
node* copy_node(node* original_node_ptr);
node* create_and_initialise_node(node_t type, union data_t data, node* left, node* right, node* parent);


#ifdef TREE_VERIFICATION

#define VERIFY_TREE(TREE_PTR, RET) if (process_tree_verification(TREE_PTR) != ok)  return RET
#define DISPLAY_TREE(TREE_PTR) generate_dump_image(TREE_PTR)

#else

# define VERIFY_TREE(TREE_PTR, RET) if (TREE_PTR == NULL) return RET
#define DISPLAY_TREE() 

#endif

#endif