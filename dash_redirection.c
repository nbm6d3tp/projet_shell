#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dash.h"

int dash_redirection(char** args, char** path_tokens, int number_args) {
  char* args_clean
      [number_args];  // Stocker les commandes à gauche de la signe "<", qui
                      // vont être exécutés. Solution trouvée à
                      // https://stackoverflow.com/questions/52939356/redirecting-i-o-in-a-custom-shell-program-written-in-c
  int cleanidx = 0;
  int input_fd = 0;
  int counter = 0;
  int original_stdin =
      dup(STDIN_FILENO);  // Stocker le STDIN original, ce qu'on va restaurer
                          // après avoir fini la redirection
  while (args[counter] != NULL) {
    if (strcmp(args[counter], "<") == 0) {
      ++counter;
      if ((input_fd = open(args[counter], O_RDONLY)) < 0 ||
          args[counter + 1] !=
              NULL) {  // Ouvrir le fichier ayant le nom indiqué dans l'argument
                       // juste après la signe "<" + Vérifier la condition
                       // "Plusieurs opérateurs de redirection ou plusieurs
                       // fichiers à droite du symbole de redirection sont des
                       // erreurs"
        write(STDERR_FILENO, error_message, strlen(error_message));
        return 1;
      }
      if (dup2(input_fd, STDIN_FILENO) == -1) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        return 1;
      }
      close(input_fd);
      break;
    }
    args_clean[cleanidx++] = args[counter];
    counter++;
  }
  args_clean[cleanidx] = NULL;
  int result = dash_launch(args_clean, path_tokens);  // Exécuter la commande
  if (dup2(original_stdin, STDIN_FILENO) ==
      -1) {  // Après avoir exécuter la commande, restaurer STDIN initial
    write(STDERR_FILENO, error_message, strlen(error_message));
    return 1;
  }
  close(original_stdin);
  return result;
}

int check_if_redirection(char* line) {
  int counter = 0;
  while (line[counter] != '\0') {
    if (line[counter] == '<') {
      return counter;
    }
    counter++;
  }
  return -1;
}