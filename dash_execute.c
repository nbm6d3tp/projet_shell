#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dash.h"

int dash_execute(char **args, char *(*path_tokens)[MAX_PATHS], int number_args,
                 int has_redirection) {
  if (args[0] == NULL) {
    return 1;
  }
  if (strcmp(args[0], "cd") == 0) {
    return dash_cd(args);
  } else if (strcmp(args[0], "exit") == 0) {
    return dash_exit(args);
  } else if (strcmp(args[0], "path") == 0) {
    return dash_path(args, path_tokens);
  } else if (strcmp(args[0], "$PATH") == 0) {
    return dash_list_paths(args, *path_tokens);
  } else {
    if (has_redirection == 1) {
      return dash_redirection(args, *path_tokens, number_args);
    } else {
      return dash_launch(args, *path_tokens);
    }
  }
}