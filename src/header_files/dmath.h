#ifndef DMATH_H
#define DMATH_H

#include "tree_properties.h"
#include "tree_dump.h" 
#include "optimizator.h"

struct diff_op_t
{
	const char* name;
	diff_ops op;
	node* (*diff_operation) (tree*, node*, char);
	double (*math_func) (double a, double b);
	bool (*optimizator_func) (tree*, node*, node**, node**, dir_t);
	void (*tex_dump_func) (node*, diff_op_t*, bool);
};

extern diff_op_t possible_ops[];


bool is_equal(double a, double b);
bool check_data_equality(node* node_ptr, double b);

node* differentiate_add   (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_sub   (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_mul   (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_div   (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_sin   (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_cos   (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_pow   (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_ln    (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_tg    (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_ctg   (tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_arcsin(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_arccos(tree* tree_ptr, node* current_node_ptr, char diff_var);

double calc_add   (double a, double b);
double calc_sub   (double a, double b);
double calc_mul   (double a, double b);
double calc_div   (double a, double b);
double calc_sin   (double a, double b);
double calc_cos   (double a, double b);
double calc_tg    (double a, double b);
double calc_ctg   (double a, double b);
double calc_pow	  (double a, double b);
double calc_ln 	  (double a, double b);
double calc_arcsin(double a, double b);
double calc_arccos(double a, double b);

bool rm_mul_node    (tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_add_node	(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_sub_node	(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_div_node    (tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_pow_node    (tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_ln_node     (tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);
bool rm_default_node(tree* tree_ptr, node* current_node, node** normal_node, node** neutral_node, dir_t branch_dir);

#endif