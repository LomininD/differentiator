#ifndef OPTIMIZATOR_H
#define OPTIMIZATOR_H

#include "differentiator_properties.h"

err_t wrap_constants(tree* tree_ptr);
err_t delete_neutral_elements(tree* tree_ptr);
void optimize_equation(tree* tree_ptr);

#endif