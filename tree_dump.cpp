#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "tree_dump.h"
#include "tree_funcs.h"

size_t node_count = 0;

static int hash(long long int ptr);
static void convert_to_image(char* code_file_name, char* image_file_name);
static void fill_preamble(FILE* fp);
static const node* list_nodes(FILE* fp, const node* current_node);
static err_t verify_node(const tree* tree, const node* node, size_t max_size);

err_t verify_tree(const tree* tree)
{
    if (tree == NULL)
    {
        printf_log_err("[from verify_tree] -> tree not found\n");
        return null_data;
    }

    int error_count = 0;

    if (tree->err_stat != ok)
    {
        printf_log_err("[from verify_tree] -> err_stat is not ok\n");
        error_count++;
    }

    node_count = 0;
    err_t nodes_ok = verify_node(tree, tree->root, tree->size);

    if (nodes_ok != ok)
    {
        error_count++;
    }

    if (node_count != tree->size)
    {
        printf_log_err("[from verify_tree] -> wrong size (should be %zu, but found %zu elements)",\
                                                                         tree->size, node_count);
        error_count++;
    }

    if (error_count != 0)
    {
        struct tree* unlocked_tree = (struct tree*) tree; // const ignored to make possible to change err_stat
        unlocked_tree->err_stat = error;
        return error;
    }

    return ok;
}


err_t verify_node(const tree* tree, const node* node, size_t max_size)
{
    if (node == NULL)
        return ok;

    node_count++;

    if (node_count > max_size)
    {
        printf_log_err("[from verify_node] -> number of nodes is greater than actual size %zu\n", max_size);
        return error;
    }

    if (node != tree->root && node->parent == NULL)
    {
        printf_log_err("[from verify_node] -> parent node pointer of non-root node [%p] is NULL\n", node);
        return error;
    }
    else if (node == tree->root && node->parent != NULL)
    {
        printf_log_err("[from verify_node] -> root node [%p] should not have parent node\n", node);
        return error;
    }
    else if (node != tree->root)
    {
        if (node->parent->left != node && node->parent->right != node)
        {
            printf_log_err("[from verify_node] -> node [%p] has wrong parent node [%p]\n", node, node->parent);
            return error;
        }
    }

    // if (node->string == NULL) // TODO - check if variables are strings not chars
    // {
    //     printf_err(debug_mode, "[from verify_node] -> node [%p] string not found\n", node);
    //     return error;
    // }

    if (verify_node(tree, node->left,  max_size) == ok && \
        verify_node(tree, node->right, max_size) == ok) return ok;
    else                                          return error;
}


err_t process_tree_verification(const tree* tree)
{
    err_t verified = verify_tree(tree);

    switch (verified)
    {
        case null_data:
            printf(MAKE_BOLD_RED("verification failed") "not enough data to show additional information\n");
            return error;
            break;
        case error:
            printf_log_err("verification failed\n"); 
            print_tree_dump(tree, "Verification failed\n");
            return error;
            break;
        case ok:
            printf_debug_msg("verification passed\n");
            return ok;
            break;
    };
}


void print_tree_dump(const tree* tree, const char* format, ...)
{
    assert(tree != NULL);

    #ifndef HTML_DEBUG_MODE

    return;

    #else

    va_list ap;
    va_start(ap, format);

    printf_log_bold("======================= TREE DUMP =======================\n\n", NULL);

    vprintf(format, ap);
    vfprintf(log_ptr, format, ap);

    printf_log_bold("\ntree [%p]\n\n", tree);
    printf_debug_msg("\terr_stat   =  %d (0 - no_error, 1 - error)\n", tree->err_stat);
    printf_debug_msg("\tsize       =  %zu\n\n", tree->size);

    printf_log_bold("=========================================================\n\n", NULL);

    generate_dump_image(tree);

    va_end(ap);
    #endif
}

#define FPRINT(...) fprintf(fp, __VA_ARGS__)

// generates tree graph and puts in html log file
void generate_dump_image(const tree* tree)
{
    assert(tree != NULL);

    #ifndef HTML_DEBUG_MODE

    return;

    #else

    static unsigned long long image_count = 0; // size_t?

    char code_file_name[file_name_size] = {};
    char image_file_name[file_name_size] = {};
    snprintf(code_file_name, file_name_size, IMAGE_CODE_PATH "%llu." IMAGE_CODE_EXT, image_count);
    snprintf(image_file_name, file_name_size, IMAGE_PATH "%llu." IMAGE_EXT, image_count);

    FILE* fp = fopen(code_file_name, "w");

    FPRINT("digraph G\n");
    FPRINT("{\n");

    fill_preamble(fp);

    FPRINT("{\n");
    FPRINT("edge[color = \"#45503B\", penwidth = 1]\n");

    if (tree->root != NULL)
         list_nodes(fp, tree->root);

    FPRINT("}\n\n");

    FPRINT("}\n");

    fclose(fp);
    image_count++;

    convert_to_image(code_file_name, image_file_name);

    printf_log_only("\n\n <img src=\"%s\"> \n\n", image_file_name);

    #endif
}


void convert_to_image(char* code_file_name, char* image_file_name)
{
    assert(code_file_name != NULL);
    assert(image_file_name != NULL);

    char sys_str[sys_str_size] = {};
    snprintf(sys_str, sys_str_size, "dot %s -T " IMAGE_EXT " -o %s", code_file_name, image_file_name); 

    //puts(sys_str);

    system(sys_str);
}


void fill_preamble(FILE* fp)
{
    assert(fp != NULL);

    // FPRINT("rankdir = LR;\n");
    FPRINT("bgcolor = \"white\"\n");
    FPRINT("node[shape = doubleoctagon, style = \"filled\", fillcolor = \"red\", fontcolor = \"white\"]\n");
}


#define PRINT_PTR(PTR) FPRINT("<FONT face = \"monospace\" color=\"#%x\"> %p </FONT>", hash((long long int) PTR), PTR)
#define BEGIN_ROW FPRINT("<TR>")
#define END_ROW FPRINT("</TR>")

const node* list_nodes(FILE* fp, const node* current_node)
{
    assert(fp != NULL);
    assert(current_node != NULL);

    //printf("listing node %p\n", current_node);

    FPRINT("NODE_%p [label = <", current_node);
    FPRINT("<TABLE BORDER=\"0\" CELLBORDER=\"1\">");

    BEGIN_ROW;
    FPRINT("<TD COLSPAN=\"2\">"); FPRINT("parent ["); PRINT_PTR(current_node->parent); FPRINT("] </TD>");
    END_ROW;

    BEGIN_ROW;
    FPRINT("<TD COLSPAN=\"2\">"); FPRINT("ptr ["); PRINT_PTR(current_node); FPRINT("] </TD>");
    END_ROW;

    BEGIN_ROW;
    FPRINT("<TD COLSPAN=\"2\">"); 
    switch (current_node->type)
    {
        case OP:
            FPRINT("%s", decode_operation_type_enum(current_node->data.operation));
            break;
        case NUM:
            FPRINT("%lf", current_node->data.number); // TODO - if number is int remove .00000
            break;
        case VAR:
            FPRINT("%c", current_node->data.variable);
            break;
        default:
            FPRINT("unknown");
    };
    FPRINT("</TD>");
    END_ROW;

    BEGIN_ROW;
    FPRINT("<TD PORT=\"f0\">"); FPRINT("left ["); PRINT_PTR(current_node->left); FPRINT("] </TD>");
    FPRINT("<TD PORT=\"f1\">"); FPRINT("right [");  PRINT_PTR(current_node->right);  FPRINT("] </TD>");
    END_ROW;

    FPRINT("</TABLE>>, shape = plain, style = filled, fillcolor = \"#C0C0C0\", fontcolor = \"black\"]\n");
        
    if (current_node->left != NULL)
    {
        const node* next_node = list_nodes(fp, current_node->left);
        FPRINT("NODE_%p:f0 -> NODE_%p\n", current_node, next_node);
    }

    if (current_node->right != NULL)
    {
        const node* next_node = list_nodes(fp, current_node->right);
        FPRINT("NODE_%p:f1 -> NODE_%p\n", current_node, next_node);
    }

    return current_node;    
}

#undef FPRINT
#undef PRINT_PTR
#undef BEGIN_ROW
#undef END_ROW


const char* decode_node_type_enum(node_t type)
{
    switch(type)
    {
        case OP:
            return "operation";
        case VAR:
            return "variable";
        case NUM:
            return "number";
        default:
            return "unknown";
    };
}


const char* decode_operation_type_enum(diff_ops op) // TODO - move to another file
{
    switch(op)
    {
        case ADD:
            return "+";
        case SUB:
            return "-";
        default:
            return "unknown operation";
    };
}


int hash(long long int ptr)
{
    int hashed = 0;

    if (ptr == 0)
    {
        return 16711680;
    }

    while (ptr != 0)
    {
        hashed += (7717 * (ptr % 10) % 1013 + 349) * 147867;
        ptr /= 10; 
    }

    hashed %= 16777215;

    hashed = hashed & 0x7f7f7f;

    return hashed;
}

