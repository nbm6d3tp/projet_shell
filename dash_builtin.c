#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dash.h"

char error_message[30] = "An error has occurred\n";

int dash_cd(char **args) {
  if (args[1] == NULL ||
      args[2] != NULL) {  //"'cd' prend toujours un argument (0 ou >1 arguments
                          // devraient être signalés comme une erreur)"
    write(STDERR_FILENO, error_message, strlen(error_message));
  } else {
    if (chdir(args[1]) != 0) {
      write(STDERR_FILENO, error_message, strlen(error_message));
    }
  }
  return 1;
}

int dash_exit(char **args) {
  if (args[1] != NULL) {  // C’est une erreur de passer des arguments à "exit"
    write(STDERR_FILENO, error_message, strlen(error_message));
    return 1;
  } else {
    exit(0);
  }
}

int dash_path(char **args, char *(*path_tokens)[MAX_PATHS]) {
  int counter = 0;
  while ((*path_tokens)[counter] != NULL) {
    free((*path_tokens)[counter]);  //"free" tous les "paths" anciens
    counter++;
  }
  counter = 1;
  while (args[counter] != NULL) {
    (*path_tokens)[counter - 1] =
        strdup(args[counter]);  // Stocker les nouveaux "paths" dans la variale
                                // globale "path_tokens"
    counter++;
  }
  (*path_tokens)[counter - 1] = NULL;
  return 1;
}

int dash_list_paths(char **args, char **path_tokens) {
  if (args[1] != NULL) {
    write(
        STDERR_FILENO, error_message,
        strlen(
            error_message));  // Erreur s'il ce commande a au moins 1 arguments
  } else {
    size_t memoryToAllocate = 7;
    char *paths_list =
        malloc(memoryToAllocate);  // La chaine de caratères qui décrit tous les
                                   // "paths" du programme
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
    write(STDOUT_FILENO, paths_list, strlen(paths_list));
    free(paths_list);
  }
  return 1;
}
