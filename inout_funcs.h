#ifndef  INOUT_FUNCS_H
#define INOUT_FUNCS_H

#include "differentiator_properties.h"

enum cmd_t
{
    calculate_derivative,
    calculate_partial_derivative,
    quit,

    unknown
};

enum ans_t
{
    no,
    yes,

    no_ans
};

cmd_t get_cmd();
int get_number();
ans_t get_answer();
char* request_string();
char get_var_name();

#endif