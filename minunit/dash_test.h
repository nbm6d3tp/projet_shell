#ifndef DASH_TEST_H
#define DASH_TEST_H
#define MAX_PATHS 50

// Les fonctionalités exactement les mêmes avec celles du programme principale,
// mais changées pour pouvoir être testées plus facilement
char **dash_split_line(char *line, int *number_args, int *has_redirection);
char **dash_split_redirection_token(char *line);
int dash_path(char **args, char *(*path_tokens)[MAX_PATHS]);
int dash_list_paths(char **args, char **path_tokens, char **result);
char *find_executable(char *command, char **path_tokens);
int dash_launch(char **args, char **path_tokens);
int check_if_redirection(char *args);
int dash_redirection(char **args, char **path_tokens, int number_args);
int dash_launch_redirection(char **args_clean, char **path_tokens);

#endif  // DASH_TEST_H
