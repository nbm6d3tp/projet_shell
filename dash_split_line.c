#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dash.h"

char **dash_split_line(char *line, int *number_args, int *has_redirection) {
  int capacity = 16;  // Initialement on peut stocker jusqu'à 16 arguments dans
                      // le tableau de "tokens"
  int counter = 0;    // Counter aide à itérer le tableau "tokens"
  char *token;  // Itérateur qui va itérer la ligne de commande tapé par
                // l'utilisateur, qui aurait été coupée en plusieurs éléments
                // par "strsep()"
  char **tokens = malloc(capacity * sizeof(char *));

  while ((token = strsep(&line, " :\n\t")) !=
         NULL) {  // Couper la ligne de commande en plusieurs éléments, basé sur
                  // les délimiteurs indiqués", et y itérer
    if (strcmp(token, "") != 0) {  // Sauter les éléments vides
      int token_has_redirection = check_if_redirection(token);
      if (token_has_redirection >= 0) {
        *has_redirection = 1;
        char **elements = dash_split_redirection_token(
            token);  // Si le "token" contient la signe de redirection "<",
                     // couper ce "token" en plusieurs "sous-tokens"
        int inner_counter = 0;
        while (elements[inner_counter] != NULL) {
          tokens[counter] =
              strdup(elements[inner_counter]);  // Stocker ces "sous-tokens"
                                                // dans le tableau de "tokens"
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
        tokens[counter] =
            strdup(token);  // Si le "token" ne contient pas la signe de
                            // redirection"<",simplement le stocker dans le
                            // tableau de "tokens"
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

char **dash_split_redirection_token(
    char *initial_token) {  // Trater et diviser un token contenant des signes
                            // de redirection "<" en plusieurs sous-tokens
  char *token;
  char **elements = NULL;
  int num_elements = 0;
  int is_last_carac_redirection =
      initial_token[strlen(initial_token) - 1] ==
      '<';  //¨Mémoriser d'avant si le "token" a la signe "<" à la fin ou pas,
            // car le initial "token" est suceptible d'être modifié par
            // "strsep()"

  if (initial_token[0] == '<') {  // Si le token a la signe "<" au début,
                                  // l'ajouter dans le tableau de résultat
    elements = realloc(elements, (num_elements + 1) * sizeof(char *));
    elements[num_elements] = strdup("<");
    num_elements++;
  }

  while ((token = strsep(&initial_token, "<")) !=
         NULL) {  // Itérer le tableau de "sous-tokens" générés par strsep() à
                  // partir du "token" initial et délimiteur "<" et stocker
                  // chaque "sous-token", suivi chacun une signe "<" aussi dans
                  // le tableau de résultat
    if (strcmp(token, "") != 0) {
      elements = realloc(elements, (num_elements + 1) * sizeof(char *));
      elements[num_elements] = strdup(token);
      num_elements++;
      elements = realloc(elements, (num_elements + 1) * sizeof(char *));
      elements[num_elements] = strdup("<");
      num_elements++;
    }
  }
  if (!is_last_carac_redirection) {  // Si le "token" initial a la signe "<" au
                                     // fin, simplement mettre NULL juste après
                                     // la fin du tableau de résultat. Sinon,
                                     // changer le dernier "sous-token" du
                                     // tableau (qui est une signe "<") à NULL
    free(elements[num_elements - 1]);
    elements[num_elements - 1] = NULL;
  } else {
    elements = realloc(elements, (num_elements + 1) * sizeof(char *));
    elements[num_elements] = NULL;
  }
  return elements;
}