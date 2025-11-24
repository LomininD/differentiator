#include <stdio.h>
#include "differentiator_funcs.h"
#include "tree_funcs.h"
#include "reader.h"


int main()
{
    initialize_log("differentiator_log.html", "DIFFERENTIATOR LOG");


    tree main_tree = {};
    tree_ctor(&main_tree);

    err_t formula_ok = read_formula(&main_tree);
    if (formula_ok != ok) return 0;

    while(true)
    {
        print_menu();

        cmd_t current_cmd = get_cmd();

        if (current_cmd == quit)
        {
            break;
        }
        

    }

    destroy_tree(&main_tree);

    end_debugging();
    return 0;
}
