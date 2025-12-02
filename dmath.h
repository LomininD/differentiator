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
};

extern diff_op_t possible_ops[];

node* differentiate_number_node(tree* tree_ptr, node* current_node_ptr);
node* differentiate_var_node(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_op_node(tree* tree_ptr, node* current_node_ptr, char diff_var);

node* differentiate_add(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_sub(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_mul(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_div(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_sin(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_cos(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_pow(tree* tree_ptr, node* current_node_ptr, char diff_var);
node* differentiate_ln (tree* tree_ptr, node* current_node_ptr, char diff_var);

double calc_add(double a, double b);
double calc_sub(double a, double b);
double calc_mul(double a, double b);
double calc_div(double a, double b);
double calc_sin(double a, double b);
double calc_cos(double a, double b);
double calc_pow(double a, double b);
double calc_ln(double a, double b);

#endif