#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dash.h"

char *find_executable(char *command, char **path_tokens) {
  char *executable_path =
      NULL;  // La chaine de caractère qui stocke le chemin exécutable pour la
             // commande tapé par utilisateur. Elle est sous forme
             // "qq_path_dans_liste_de_paths/commande"
  int counter = 0;
  while (path_tokens[counter] != NULL) {
    executable_path = realloc(
        executable_path,
        (strlen(command) + 2 + strlen(path_tokens[counter])) * sizeof(char));
    strcpy(executable_path, path_tokens[counter]);
    strcat(executable_path, "/");
    strcat(executable_path, command);
    if (access(executable_path, X_OK) ==
        0) {  // Vérifier si le chemin est vraiment exécutable
      return executable_path;
    }
    free(executable_path);
    counter++;
  }
  return NULL;
}

int dash_launch(char **args, char **path_tokens) {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0) {
    // Processe enfant
    char *executable_path = find_executable(args[0], path_tokens);
    if (executable_path != NULL) {
      if (execv(executable_path, args) == -1) {  // Exécuter la commande
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
      free(executable_path);
    } else {
      write(STDERR_FILENO, error_message, strlen(error_message));
    }
  } else if (pid < 0) {
    // fork() erreur
    write(STDERR_FILENO, error_message, strlen(error_message));
  } else {
    // Processe parent
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}