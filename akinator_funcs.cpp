// #include <sys/stat.h>
// #include <assert.h>
// #include <stdlib.h>
// #include <string.h>
// #include "akinator_funcs.h"
// 
// 
// static err_t check_prediction(tree* tree, node* current_node);
// static void write_node(FILE* save_ptr, const tree* tree, const node* current_node);
// static err_t process_saving(const tree* tree);
// static err_t process_loading(tree* tree);
// static node* read_node(char** current_pos, md_t debug_mode, size_t* node_count);
// static void skip_spaces(char** current_pos);
// 
// cmd_t request_cmd(const tree* tree)
// {
//     VERIFY_TREE(unknown);
//     
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "\n");
//     printf_log_msg(debug_mode, "request_cmd: began requesting command\n");
// 
//     say_message(debug_mode, "-> Choose a command to execute\n", "Choose a command to execute", NULL);
//     printf_both(debug_mode, "-> [g]uess object, [s]ave base, [l]oad base, [q]uit\n");
// 
//     cmd_t current_cmd = get_cmd(debug_mode);
// 
//     printf_log_msg(debug_mode, "request_cmd: finished requesting command\n\n");
// 
//     return current_cmd;
// }
// 
// 
// err_t process_guessing(tree* tree)
// {
//     VERIFY_TREE(error); // FIXME - copy pasting
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "guess_object: began guessing object\n");
//     
//     if (tree->size == 0)
//     {
//         err_t requested_beginning = request_tree_beginning(tree);
//         return requested_beginning;
//     }
// 
//     ans_t ans = no_ans;
//     bool guessed = false;
//     node* current_node = tree->root;
// 
//     while(!guessed)
//     {
//         say_message(debug_mode, "-> %s? ([y]es / [n]o)\n", "%s", current_node->string);
// 
//         ans = get_answer(debug_mode);
// 
//         if (ans == yes) current_node = current_node->yes_branch;
//         else            current_node = current_node->no_branch;
// 
//         if (current_node->yes_branch == NULL)   
//         {
//             err_t checked = check_prediction(tree, current_node);
//             if (checked != ok) return checked;
//             guessed = true;
//         }
//     }
// 
//     VERIFY_TREE(error);
// 
//     printf_log_msg(debug_mode, "guess_object: finished guessing object\n");
// 
//     return ok;
// }
// 
// 
// err_t save_database(const tree* tree)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "save_database: began saving database\n");
// 
//     say_message(debug_mode, "-> Saving current version of database will overwrite saved version of it.\n", \
//                             "Saving current version of database will overwrite saved version of it", NULL);
//     say_message(debug_mode, "-> Save anyway? ([y]es / [n]o)\n", "Save anyway", NULL);
// 
//     ans_t ans = get_answer(debug_mode);
//     err_t saved = ok; // FIXME - how to remove
// 
//     switch (ans)
//     {
//         case yes:
//             saved = process_saving(tree);
//             if (saved != ok) return saved;
//             break;
//         case no:
//             break;
//         default:
//             return error;
//     };
// 
//     printf_log_msg(debug_mode, "save_database: finished process\n");
// 
//     return ok;
// }
// 
// 
// err_t process_saving(const tree* tree)
// {
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "\n");
//     printf_log_msg(debug_mode, "save_database: started writing in file\n");
// 
//     FILE* save_ptr = fopen(SAVE_FILE_NAME, "w");
//     write_node(save_ptr, tree, tree->root);
//     fclose(save_ptr);
//     say_message(debug_mode, "-> Database saved successfully\n", "Database saved successfully", NULL);
// 
//     printf_log_msg(debug_mode, "save_database: done writing in file\n\n");
//     return ok;
// }
// 
// #define FPRINT(...) fprintf(save_ptr, __VA_ARGS__)
// 
// void write_node(FILE* save_ptr, const tree* tree, const node* current_node)
// {
//     printf_log_msg(tree->debug_mode, "writing [%p]\n", current_node);
// 
//     if (current_node == NULL) { FPRINT("nil "); return; }
//     
//     FPRINT("( ");
//     FPRINT("\"%s\" ", current_node->string);
//     write_node(save_ptr, tree, current_node->yes_branch);
//     write_node(save_ptr, tree, current_node->no_branch);
//     FPRINT(") ");
// }
// 
// #undef FPRINT
// 
// err_t load_database(tree* tree)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "load_database: process began\n");
// 
//     say_message(debug_mode, "-> Loading saved version of database will overwrite current version of it.\n",\
//                      "Loading saved version of database will overwrite current version of it", NULL);
//     say_message(debug_mode, "-> Load anyway? ([y]es / [n]o)\n", "Load anyway", NULL);
// 
//     ans_t ans = get_answer(debug_mode);
// 
//     switch (ans)
//     {
//         case yes:
//             process_loading(tree);
//             break;
//         case no:
//             break;
//         default:
//             return error;
//     };
// 
//     VERIFY_TREE(error);
// 
//     print_tree_dump(tree, "Displaying freshly read tree", NULL);
//     printf_log_msg(debug_mode, "load_database: process finished\n");
//     return ok;
// }
// 
// 
// err_t process_loading(tree* tree)
// {
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "process_loading: process started\n");
// 
//     FILE* load_ptr = fopen(SAVE_FILE_NAME, "r");
//     
//     if (load_ptr == NULL)
//     {
//         say_message(debug_mode, "-> Could not find the database save file.", "Could not find the database save file", NULL);
//         say_message(debug_mode, "-> It seems that database has not been saved before.", "It seems that database has not been saved before", NULL);
//         return ok;
//     }
// 
//     struct stat file_info = {};
//     fstat(fileno(load_ptr), &file_info);
//     long long bytes_in_file = file_info.st_size / sizeof(char);
// 
//     char* text_buf = (char*) calloc (bytes_in_file + 1, sizeof(char));
// 
//     if (text_buf == NULL) 
//     {
//         printf_err(debug_mode, "[from process_loading] -> not enough memory for file data\n");
//         return error;
//     }
// 
//     size_t bytes_read = fread(text_buf, sizeof(char), bytes_in_file, load_ptr);
//     *(text_buf + bytes_in_file) = '\0';
// 
//     tree->text_buf = text_buf;
// 
//     printf_log_msg(debug_mode, "process_loading: bytes in file: %lld\n", bytes_in_file);
//     printf_log_msg(debug_mode, "process_loading: bytes read:    %zu\n", bytes_read);
// 
//     size_t node_count = 0;
// 
//     node* root_node = read_node(&text_buf, debug_mode, &node_count);
//     tree->root = root_node;
//     tree->size = node_count;
// 
//     fclose(load_ptr);
// 
//     //free(text_buf);
//     say_message(debug_mode, "-> Database loaded successfully\n", "Database loaded successfully", NULL);
//     printf_log_msg(debug_mode, "process_loading: process finished\n");
// 
//     return ok;
// }
// 
// 
// node* read_node(char** current_pos, md_t debug_mode, size_t* node_count)
// {
//     skip_spaces(current_pos);
//     if (**current_pos == '(')
//     {
//         printf_log_msg(debug_mode, "read_node: got '(', began reading node\n");
//         printf_log_msg(debug_mode, "%s", *current_pos);
// 
//         node* new_node = create_node(debug_mode);
//         if (new_node == NULL) return NULL;
//         new_node->freshly_created = false;
//         (*node_count)++;
// 
//         (*current_pos)++;
//         skip_spaces(current_pos);
// 
//         printf_log_msg(debug_mode, "read_node: started reading node name\n");
//         printf_log_msg(debug_mode, "%s", *current_pos);
// 
//         int name_len = 0;
//         sscanf(*current_pos, "\"%*[^\"]\"%n", &name_len);
// 
//         printf_log_msg(debug_mode, "read_node: done reading node name\n");
//         printf_log_msg(debug_mode, "%s", *current_pos);
// 
//         *strchr(*current_pos + 1, '"') = '\0';
//         new_node->string = *current_pos + 1;
// 
//         *current_pos += name_len;
// 
//         printf_log_msg(debug_mode, "read_node: node name assigned\n");
//         printf_log_msg(debug_mode, "%s", *current_pos);
// 
//         skip_spaces(current_pos);
// 
//         printf_log_msg(debug_mode, "read_node: began reading left subtree\n");
//         printf_log_msg(debug_mode, "%s", *current_pos);
// 
//         node* left_node = read_node(current_pos, debug_mode, node_count);
//         new_node->yes_branch = left_node;
// 
//         skip_spaces(current_pos);
// 
//         printf_log_msg(debug_mode, "read_node: began reading right subtree\n");
//         printf_log_msg(debug_mode, "%s", *current_pos);
// 
//         node* right_node = read_node(current_pos, debug_mode, node_count);
//         new_node->no_branch = right_node;
// 
//         skip_spaces(current_pos);
// 
//         if (**current_pos != ')')
//         {
//             printf_err(debug_mode, "[from read_node] -> corrupted save file (no ')')\n");
//             destroy_node(new_node, debug_mode);
//             return NULL;
//         }
// 
//         (*current_pos)++;
// 
//         printf_log_msg(debug_mode, "read_node: node is read\n");
//         printf_log_msg(debug_mode, "%s", *current_pos);
//         
//         return new_node;
//     }
//     else if (**current_pos == 'n' && *(*current_pos + 1) == 'i' && *(*current_pos + 2) == 'l')
//     {
//         *current_pos += 3;
// 
//         printf_log_msg(debug_mode, "read_node: got nil\n");
//         printf_log_msg(debug_mode, "%s", *current_pos);
// 
//         return NULL;
//     }
//     else
//     {
//         printf_err(debug_mode, "[from read_node] -> corrupted save file (no '(' or nil)\n");
//         return NULL;
//     }
// }
// 
// 
// void skip_spaces(char** current_pos)
// {
//     while (**current_pos == ' ')
//     {
//         (*current_pos)++;
//     }
// }
// 
// 
// err_t check_prediction(tree* tree, node* current_node)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "check_prediction: began checking guessed object\n");
// 
//     say_message(debug_mode, "-> This is %s, am I right? ([y]es / [n]o)\n", "This is %s, am I right",  current_node->string);
//     ans_t ans = get_answer(debug_mode);
//     err_t requested = ok; // FIXME - not supposed to be here
// 
//     switch (ans)
//     {
//         case yes:
//             say_message(debug_mode, "-> Yeah, I am always right.\n", "Yeah, I am always right", NULL);
//             return ok;
//         case no:
//             requested = request_new_nodes(tree, current_node);
//             if (requested != ok) return requested;
//             return ok;
//         default:
//             return error;
//     };
// }
// 
// 
// // err_t give_definition(tree* tree, char* obj_name)
// // {
// //     VERIFY_TREE(error);
// //     assert(obj_name != NULL);
// // }
// // 
// // 
// // node* search_obj(tree* tree, node* current_node, const char* name)
// // {
// //     if (current_node == NULL)
// //         return NULL;
// //     if (strcmp(name, current_node->string) == 0)
// //         return current_node;
// //     
// //     node* found_node = search_obj(tree, current_node->yes_branch, name);
// //     if (found_node == NULL)
// //         found_node = search_obj(tree, current_node->no_branch, name);
// //         
// // }
// 
// 
// err_t request_tree_beginning(tree* tree)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "request_new_node: began requesting tree beginning\n");
// 
//     say_message(debug_mode, "-> Hm... It seems that there is no data, where can it be?\n", \
//                         "Hm... It seems that there is no data, where can it be", NULL);
//     say_message(debug_mode, "-> Lets create and set up database now.\n", \
//                     "Lets create and set up data base now", NULL);
// 
//     say_message(debug_mode, "-> Enter first object name, please\n", "Enter first object name, please", NULL); // FIXME - copy pasting
//     char* first_obj_name = request_string(debug_mode);
//     if (first_obj_name == NULL) return error;
//     
//     say_message(debug_mode, "-> Enter second object name, please:\n", "Enter second object name, please", NULL);
//     char* second_obj_name = request_string(debug_mode);
//     if (second_obj_name == NULL) return error;
// 
//     node* yes_node = create_node(debug_mode);
//     node* no_node  = create_node(debug_mode);
// 
//     yes_node->string = first_obj_name;
//     no_node->string  = second_obj_name;
// 
//     yes_node->freshly_created = true;
//     no_node->freshly_created  = true;
// 
//     say_message(debug_mode, "-> How does %s differ from %s?\n", "How does %s differ from %s", first_obj_name, second_obj_name);
//     char* diff_str = request_string(debug_mode);
//     
//     node* root_node = create_node(debug_mode);
//     root_node->string = diff_str;
//     root_node->yes_branch = yes_node;
//     root_node->no_branch = no_node;
//     root_node->freshly_created = true;
// 
//     tree->root = root_node;
//     tree->size = 3;
//     
//     VERIFY_TREE(error);
// 
//     printf_log_msg(debug_mode, "request_new_node: tree beginning requested\n");
// 
//     DISPLAY_TREE();
// 
//     return ok;
// }
// 
// 
// err_t request_new_nodes(tree* tree, node* parent_node)
// {
//     VERIFY_TREE(error);
// 
//     md_t debug_mode = tree->debug_mode;
// 
//     printf_log_msg(debug_mode, "request_new_node: began requesting new nodes\n");
// 
//     say_message(debug_mode, "-> What is it?\n", "What is it", NULL);
// 
//     char* obj_name  = request_string(debug_mode);
// 
//     if (obj_name == NULL)
//         return error;
// 
//     say_message(debug_mode, "-> How does it differ from %s?\n", "How does it differ from %s", parent_node->string);
// 
//     char* diff_string  = request_string(debug_mode);
// 
//     if (diff_string == NULL)
//         return error;
// 
//     node* new_left_node = create_node(debug_mode);
//     node* new_right_node = create_node(debug_mode);
// 
//     new_right_node->string = parent_node->string;
//     new_left_node->string = obj_name;
//     parent_node->string = diff_string;
//     parent_node->yes_branch = new_left_node;
//     parent_node->no_branch = new_right_node;
//     parent_node->freshly_created = true;
//     new_left_node->freshly_created = true;
// 
//     tree->size += 2;

//
// 
//     VERIFY_TREE(error);
//     
//     printf_log_msg(debug_mode, "request_new_node: new nodes are requested\n");
// 
//     DISPLAY_TREE()
//     return ok;
// }
