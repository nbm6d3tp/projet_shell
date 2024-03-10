#define _GNU_SOURCE

#include "dash_test.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Les fonctionalités exactement les mêmes avec celles du programme principale,
// mais changées pour pouvoir être testées plus facilement

char *find_executable(char *command, char **path_tokens) {
  char *executable_path = NULL;
  int counter = 0;
  while (path_tokens[counter] != NULL) {
    executable_path = realloc(
        executable_path,
        (strlen(command) + 2 + strlen(path_tokens[counter])) * sizeof(char));
    strcpy(executable_path, path_tokens[counter]);
    strcat(executable_path, "/");
    strcat(executable_path, command);
    if (access(executable_path, X_OK) == 0) {
      return executable_path;
    }
    free(executable_path);
    counter++;
  }
  return NULL;
}

int check_if_redirection(char *line) {
  int counter = 0;
  while (line[counter] != '\0') {
    if (line[counter] == '<') {
      return counter;
    }
    counter++;
  }
  return -1;
}

int dash_path(char **args, char *(*path_tokens)[MAX_PATHS]) {
  int counter = 0;
  while ((*path_tokens)[counter] != NULL) {
    free((*path_tokens)[counter]);
    counter++;
  }
  counter = 1;
  while (args[counter] != NULL) {
    (*path_tokens)[counter - 1] = strdup(args[counter]);
    counter++;
  }
  (*path_tokens)[counter - 1] = NULL;
  return 1;
}

int dash_list_paths(char **args, char **path_tokens, char **result) {
  if (args[1] != NULL) {
    return 1;
  } else {
    size_t memoryToAllocate = 7;
    char *paths_list = malloc(memoryToAllocate);
    int counter = 0;
    strcpy(paths_list, "dash: ");
    while (path_tokens[counter] != NULL) {
      memoryToAllocate += sizeof(path_tokens[counter]) + 1;
      paths_list = realloc(paths_list, memoryToAllocate);
      strcat(paths_list, path_tokens[counter]);
      if ((path_tokens[counter + 1] != NULL)) {
        strcat(paths_list, ":");
      }
      counter++;
    }
    strcat(paths_list, "\n");
    *result = paths_list;
  }
  return 0;
}

char **dash_split_line(char *line, int *number_args, int *has_redirection) {
  char *token;
  int capacity = 16;
  char **tokens = malloc(capacity * sizeof(char *));
  int counter = 0;
  while ((token = strsep(&line, " :\n\t")) != NULL) {
    if (strcmp(token, "") != 0) {
      int token_has_redirection = check_if_redirection(token);
      if (token_has_redirection >= 0) {
        *has_redirection = 1;
        char **elements = dash_split_redirection_token(token);
        int inner_counter = 0;
        while (elements[inner_counter] != NULL) {
          tokens[counter] = strdup(elements[inner_counter]);
          counter++;
          if (counter >= capacity) {
            capacity = (int)(capacity * 1.5);
            tokens = realloc(
                tokens,
                capacity *
                    sizeof(char *));  //à ce moment là, le tableau de chaines de
                                      // caratères "tokens" est plein <=> la
                                      // taille du tableau de tokens est plus
                                      // grande que 16, la taille du tableau de
                                      // chaines de caratères qu'on a supposé
                                      // initiallement. Du coup, on augemente la
                                      // capacité de ce tableau de 50%. La zone
                                      // de mémoire pour ce tableau est
                                      // ré-alloué à l'aide de realloc() pour
                                      // s'adapter à la nouvelle capacité.
          }
          free(elements[inner_counter]);
          inner_counter++;
        }
        free(elements);
      } else {
        tokens[counter] = strdup(token);
        counter++;
        if (counter >= capacity) {
          capacity = (int)(capacity * 1.5);
          tokens = realloc(
              tokens,
              capacity *
                  sizeof(
                      char *));  //à ce moment là, le tableau de chaines de
                                 // caratères "tokens" est plein <=> la taille
                                 // du tableau de tokens est plus grande que 16,
                                 // la taille du tableau de chaines de caratères
                                 // qu'on a supposé initiallement. Du coup, on
                                 // augemente la capacité de ce tableau de 50%.
                                 // La zone de mémoire pour ce tableau est
                                 // ré-alloué à l'aide de realloc() pour
                                 // s'adapter à la nouvelle capacité.
        }
      }
    }
  }
  tokens[counter] = NULL;
  *number_args = counter;
  return tokens;
}
char **dash_split_redirection_token(char *line) {
  char *token;
  char **elements = NULL;
  int num_elements = 0;
  int is_last_carac_redirection = line[strlen(line) - 1] == '<';

  if (line[0] == '<') {
    elements = realloc(elements, (num_elements + 1) * sizeof(char *));
    elements[num_elements] = strdup("<");
    num_elements++;
  }

  while ((token = strsep(&line, "<")) != NULL) {
    if (strcmp(token, "") != 0) {
      elements = realloc(elements, (num_elements + 1) * sizeof(char *));
      elements[num_elements] = strdup(token);
      num_elements++;
      elements = realloc(elements, (num_elements + 1) * sizeof(char *));
      elements[num_elements] = strdup("<");
      num_elements++;
    }
  }
  if (!is_last_carac_redirection) {
    free(elements[num_elements - 1]);
    elements[num_elements - 1] = NULL;
  } else {
    elements = realloc(elements, (num_elements + 1) * sizeof(char *));
    elements[num_elements] = NULL;
  }
  return elements;
}

int dash_launch(char **args, char **path_tokens) {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0) {
    char *executable_path = find_executable(args[0], path_tokens);
    if (executable_path != NULL) {
      int fd = open("output_ls.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
      dup2(fd, STDOUT_FILENO);
      if (execv(executable_path, args) == -1) {
        return 1;
      }
      close(fd);
    } else {
      exit(EXIT_FAILURE);
    }
  } else if (pid < 0) {
    return 1;
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    return status;
  }
}

int dash_launch_redirection(char **args_clean, char **path_tokens) {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0) {
    // Child process
    char *executable_path = find_executable(args_clean[0], path_tokens);
    if (executable_path != NULL) {
      int fd =
          open("output_redirection.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
      dup2(fd, STDOUT_FILENO);
      if (execv(executable_path, args_clean) == -1) {
        return 1;
      }
      close(fd);
      free(executable_path);
    } else {
      return 1;
    }
  } else if (pid < 0) {
    return 1;
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    return status;
  }
  return 1;
}

int dash_redirection(char **args, char **path_tokens, int number_args) {
  char *args_clean[number_args];
  int cleanidx = 0;
  int input_fd = 0;
  int counter = 0;
  int original_stdin = dup(STDIN_FILENO);
  while (args[counter] != NULL) {
    if (strcmp(args[counter], "<") == 0) {
      ++counter;
      if ((input_fd = open(args[counter], O_RDONLY)) < 0 ||
          args[counter + 1] != NULL) {
        return 1;
      }
      if (dup2(input_fd, STDIN_FILENO) == -1) {
        return 1;
      }
      close(input_fd);
      break;
    }
    args_clean[cleanidx++] = args[counter];
    counter++;
  }
  args_clean[cleanidx] = NULL;
  int result = dash_launch_redirection(args_clean, path_tokens);
  if (dup2(original_stdin, STDIN_FILENO) == -1) {
    return 1;
  }
  close(original_stdin);
  return result;
}