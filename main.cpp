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


    node* cn = copy_node(main_tree_ptr, main_tree_ptr->root);
    printf("root %p\n", main_tree_ptr->root);
    printf("CN %p\n", cn);

    tree* copied_tree = tree_ctor();
    copied_tree->root = cn;
    copied_tree->size = 0;

    print_tree_dump(copied_tree, "copied tree\n");

    destroy_tree(main_tree_ptr);
    destroy_tree(copied_tree);
    end_debugging();
    return 0;
}
