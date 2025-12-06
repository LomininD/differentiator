#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../header_files/inout_funcs.h"


static void clear_buffer(void);
static bool check_buffer();


cmd_t get_cmd()
{
    char ans = 0;
    bool got_ans = false;
    int scanned = 0;

    while(!got_ans)
    {
        scanned = scanf("%c", &ans);

        if (scanned != 1 || !check_buffer())
        {
            printf_both("-> Could not read the answer, please try again:\n");
            clear_buffer();
            clearerr(stdin);
            continue;
        }

        printf_debug_msg("get_answer: got \"%c\"\n", ans);

        switch (ans)
        {
            case 'q':
                return quit;
            case 'd':
                return calculate_derivative;
            case 'p':
                return calculate_partial_derivative;
            default:
                printf_both("-> Could not recognize the answer, please try again:\n");
                continue;
        };
    }

    return unknown;
}


char get_var_name()
{
    bool got_ans = false;
    char* var = NULL;

    while (!got_ans)
    {
        var = request_string();
        if (strlen(var) != 1)
        {
            printf_both("-> variable name is too big, please try again\n");
            continue;
        }
        got_ans = true;
    }

    char var_name = var[0]; // TODO - change for multi-char vars
    free(var);

    return var_name;
}


double request_number()
{
    bool got_ans = false;
    double ans = 0;

    while (!got_ans)
    {
        int scanned = scanf("%lg", &ans);

        printf_debug_msg("got %lg\n", ans);

        if (scanned != 1 || !check_buffer())
        {
            printf_both("-> Could not recognize the answer, please try again:\n");
            clear_buffer();
            continue;
        }

        got_ans = true;
    }

    return ans;
}


ans_t get_answer()
{
    char ans = 0;
    bool got_ans = false;
    int scanned = 0;

    while(!got_ans)
    {
        scanned = scanf("%c", &ans);

        if (scanned != 1 || !check_buffer())
        {
            printf_both("-> Could not read the answer, please try again:\n");
            clear_buffer();
            clearerr(stdin);
            continue;
        }

        printf_debug_msg("get_answer: got \"%c\"\n", ans);

        switch (ans)
        {
            case 'y':
                return yes;
            case 'n':
                return no;
            default:
                printf_both("-> Could not recognize the answer, please try again:\n");
                continue;
        };
    }

    return no_ans;
}


// FIXME - should ask again if got only '\n' remove excess spaces
char* request_string() 
{
    char* string = (char*) calloc(100, sizeof(string));

    if (string == NULL)
    {
        printf_log_err("[from request_string] -> could not allocate memory for the string\n");
        return NULL;
    }

    size_t string_len = 0;
    ssize_t read = 0;

    read = getline(&string, &string_len, stdin);
    
    while (read <= 0)
    {
        printf_both("-> Bad answer (got nothing), please try again\n");
        clearerr(stdin);
        read = getline(&string, &string_len, stdin);
    }

    *strrchr(string, '\n') = '\0';

    printf_debug_msg("request_string: got \"%s\"\n", string);

    return string;
}


static void clear_buffer(void)
{
    int c = '\0';
    while ((c = getchar()) != '\n' && c != EOF);
    //printf("clear_buffer: cleared\n");
}

static bool check_buffer()
{
    int c = 0;
    while ((c = getchar()) != EOF && c != '\n')
        if (c != ' ' && c != '\n' && c != '\t')
            return false;
    return true;
}

