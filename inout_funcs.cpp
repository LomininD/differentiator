#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "inout_funcs.h"

static bool check_ans(char* ans);
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
            default:
                printf_both("-> Could not recognize the answer, please try again:\n");
                continue;
        };
    }

    return unknown;
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
        printf_log_err("[from request_string] -> could not allocate memory for node data\n");
        return NULL;
    }

    size_t string_len = 0;
    int read = 0;

    printf("Hello world");




    read = getline(&string, &string_len, stdin);
    
    while (read <= 0 || !check_ans(string))
    {
        printf_both("-> Bad answer (got nothing or got \"not\" in the answer), please try again\n");
        clearerr(stdin);
        read = getline(&string, &string_len, stdin); // clear_buffer()?
    }

    *strrchr(string, '\n') = '\0';

    printf_debug_msg("request_string: got \"%s\"\n", string);

    return string;
}


bool check_ans(char* ans)
{
    assert(ans != NULL);

    char* cur_pos = ans;
    char* word = (char*) calloc(strlen(ans), sizeof(char*));
    size_t read_symb = 0;
    int read_stat = 1;

    while (read_stat > 0)
    {
        read_stat = sscanf(cur_pos, "%s%n", word, &read_symb);
        if (strcmp(word, "not") == 0)
            return false;
        cur_pos += read_symb;
    }

    free(word);
    return true;
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

