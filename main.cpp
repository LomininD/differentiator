#include <stdio.h>
#include "differentiator_funcs.h"
#include "tree_funcs.h"
#include "tree_dump.h"


int main()
{
    initialize_log("differentiator_log.html", "DIFFERENTIATOR LOG");

    tree main_tree = {};


    tree_ctor(&main_tree);

    read_formula(&main_tree);

    print_tree_dump(&main_tree, "Main tree view");

    destroy_tree(&main_tree);

    end_debugging();
    return 0;
}
