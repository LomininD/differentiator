#ifndef TREE_DUMP_H
#define TREE_DUMP_H

#include "tree_properties.h"

const size_t file_name_size = 64;
const size_t sys_str_size = 200;

void print_tree_dump(const tree* tree, const char* format, ...);
void generate_dump_image(const tree* tree);

const char* decode_node_type_enum(node_t type);

err_t verify_tree(const tree* tree);
err_t process_tree_verification(const tree* tree);


#define IMAGE_CODE_EXT "txt"
#define IMAGE_EXT "svg"
#define IMAGE_CODE_PATH "dump_img/code/"
#define IMAGE_PATH "dump_img/"

#endif