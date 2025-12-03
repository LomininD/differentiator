#ifndef READER_H
#define READER_H

#include "differentiator_properties.h"

err_t read_formula(tree* tree);
err_t read_text_buf(FILE* file_ptr, char** text_buf_ptr);
void skip_spaces(char** current_pos);

#endif