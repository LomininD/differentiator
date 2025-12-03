#ifndef TEX_DUMP_H
#define TEX_DUMP_H

#include "tree_properties.h"
#include "dmath.h"

void initialise_tex_file();
void fill_main_equation_preamble(tree* tree_ptr);
void dump_end_main_equation_preamble();

void fill_derivative_preamble();
void dump_start_of_differentiation(node* node_ptr, char diff_var);
void dump_intermediate_calculations(node* node_ptr);
void dump_end_of_differentiation();

void close_tex_file();
void convert_to_pdf();

void dump_add_sub	(node* node_ptr, diff_op_t* op_struct);
void dump_mul    	(node* node_ptr, diff_op_t* op_struct);
void dump_div	 	(node* node_ptr, diff_op_t* op_struct);
void dump_unary_func(node* node_ptr, diff_op_t* op_struct);
void dump_pow	 	(node* node_ptr, diff_op_t* op_struct);




#endif