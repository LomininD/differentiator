#include <stdio.h>
#include "differentiator_funcs.h"
#include "tree_funcs.h"
#include "reader.h"
#include "advanced_reader.h"
#include <stdlib.h>
#include "tex_dump.h"

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
    #endif
    if (formula_ok != ok) return 0;

    fill_main_equation_preamble(main_tree_ptr);
    optimize_equation(main_tree_ptr);
    dump_end_main_equation_preamble();

    dump_name_table();

    //bool end = false;

    fill_derivative_preamble();

    process_calculating_partial_derivative(main_tree_ptr);

//     while(!end)
//     {
//         print_menu();
// 
//         cmd_t current_cmd = get_cmd();
//         err_t executed = ok;
// 
//         switch (current_cmd)
//         {
//             case calculate_partial_derivative:
//                 executed = process_calculating_partial_derivative(main_tree_ptr);
//                 break;
//             case quit:
//                 end = true;
//                 break;
//         };
//     }
    
    close_tex_file();
    convert_to_pdf();
    destroy_tree(main_tree_ptr);
    end_debugging();
    return 0;
}
