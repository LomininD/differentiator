#include <stdio.h>
#include "differentiator_funcs.h"
#include "tree_funcs.h"
#include "reader.h"
#include <stdlib.h>


int main()
{
    initialize_log("differentiator_log.html", "DIFFERENTIATOR LOG");

    tree* main_tree_ptr = tree_ctor();
    if (global_err_stat != ok) return 0;

    err_t formula_ok = read_formula(main_tree_ptr);
    if (formula_ok != ok) return 0;

    wrap_constants(main_tree_ptr); // TODO - remove

    bool end = false;

    while(!end)
    {
        print_menu();

        cmd_t current_cmd = get_cmd();
        err_t executed = ok;

        switch (current_cmd)
        {
            case calculate_partial_derivative:
                executed = process_calculating_partial_derivative(main_tree_ptr);
                break;
            case quit:
                end = true;
                break;
        };
    }


    destroy_tree(main_tree_ptr);
    end_debugging();
    return 0;
}
