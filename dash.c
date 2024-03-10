#include "dash.h"
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// "__attribute__" indique au compilateur de ne pas afficher de warning si
// la variable n'est pas utilisée.
int main(int ac, char **av __attribute__((unused))) {
  if (ac > 1) {  // "L’interpréteur de commandes peut uniquement être invoqué
                 // sans argument. Tout autre argument résulte en une erreur."
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(EXIT_FAILURE);
  }

  char *path_tokens[MAX_PATHS];  // La variable du scope globale, qui stocke la
                                 // liste des "path" de l'application.
  for (int i = 0; i < MAX_PATHS;
       i++) {  // Initialiser le tableau avec des valeurs NULL, qui va aider à
               // parcourir le tableau dans l'avenir (par exemple
               // while(path_tokens[counter]!=NULL) ...)
    path_tokens[i] = NULL;
  }
  path_tokens[0] =
      strdup("/bin");  //"Le chemin initial de votre interpréteur de commandes
                       // doit contenir un répertoire : /bin"

  while (1) {
    int int_mode =
        isatty(STDIN_FILENO);  //Éviter la boucle infinie après utiliser la
                               // fonctionalité redirection: Quand on utilise la
                               // redirection, isatty(STDIN_FILENO) va retourner
                               // une valeur différent que 1, donc on n'entrera
                               // pas dedans "if". Dans la fonction
                               // dash_redirection va nous fournir un moyen pour
                               // re-entrer dans le le programme plus sûr
    if (int_mode == 1) {
      char current_directory[100];
      int number_args = 0;
      int has_redirection = 0;
      printf("%s: dash> ",
             getcwd(current_directory,
                    100));  // Afficher l’invite de taper "dash>" dans le
                            // terminal et notre position actuellement
      char *line = dash_read_line();
      char **tokens = dash_split_line(line, &number_args, &has_redirection);

      if (tokens[0] != NULL) {
        dash_execute(tokens, &path_tokens, number_args,
                     has_redirection);  // Executer la commande
      }

      int free_up_counter = 0;
      while (tokens[free_up_counter] != NULL) {
        free(tokens[free_up_counter]);
        free_up_counter++;
      }
      free(tokens);
      free(line);
    }
  }
  int counter = 0;
  while (path_tokens[counter] != NULL) {
    free(path_tokens[counter]);
    counter++;
  }
}