#include <stdio.h>
#include <stdlib.h>
#include "../header_files/differentiator_funcs.h"
#include "../header_files/tree_funcs.h"
#include "../header_files/reader.h"
#include "../header_files/advanced_reader.h"
#include "../header_files/tex_dump.h"

#define USE_ADVANCED_READER

int main()
{
    initialize_log("differentiator_log.html", "DIFFERENTIATOR LOG");
    initialise_tex_file();

    tree* main_tree_ptr = tree_ctor();
    if (global_err_stat != ok) return 0;

    # if defined(USE_ADVANCED_READER)
    err_t formula_ok = advanced_read_formula(main_tree_ptr);
    # else
    err_t formula_ok = read_formula(main_tree_ptr);
    # endif

    if (formula_ok != ok) return 0;

    fill_main_equation_preamble(main_tree_ptr);
    optimize_equation(main_tree_ptr);
    dump_end_main_equation_preamble();

    ask_for_variable_values();

    dump_name_table();

    process_calculating_value(main_tree_ptr->root);

    fill_derivative_preamble();

    process_calculating_partial_derivative(main_tree_ptr);

    fill_tangent_preamble();
    
    close_tex_file();
    convert_to_pdf();

    shut_down_differentiator();
    end_debugging();
    return 0;
}
