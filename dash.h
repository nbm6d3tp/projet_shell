#ifndef DASH_H
#define DASH_H
#define MAX_PATHS 50  // Nombre maximum de "paths" du programme

extern char error_message[30];  // "Vous devez affcher ce seul et unique message
                                // d’erreur chaque fois que vous rencontrez une
                                // erreur, quel qu’en soit le type."

int dash_execute(
    char **args, char *(*path_tokens)[MAX_PATHS], int number_args,
    int has_redirection);  // Contrôler quelle fonctionalité à lancer suite à la
                           // commande tapée de l'utilisateur

char *dash_read_line();  // Attendre et lire la commande d'utilisateur

char **dash_split_line(
    char *line, int *number_args,
    int *has_redirection);  // Traiter la commande tapée par l'utilisateur:
                            // diviser la ligne de commande tapée sous forme
                            // d'une liste de "tokens", retourner le nombre
                            // d'arguments et si la ligne de commande contient
                            // la signe de redirection

char **dash_split_redirection_token(
    char *line);  // Trater et diviser un token contenant des signes de
                  // redirection "<" en plusieurs sous-tokens

int dash_cd(char **args);  // Traiter la commande intégrée "cd"

int dash_exit(char **args);  // Traiter la commande intégrée "exit"

int dash_path(
    char **args,
    char *(*path_tokens)[MAX_PATHS]);  // Traiter la commande intégrée "path"

int dash_list_paths(
    char **args,
    char **path_tokens);  // Traiter la commande intégrée qui
                          // liste le contenu de la variable
                          // "$PATH" (fonctionalité ajoutée en plus)

char *find_executable(
    char *command,
    char **path_tokens);  // Chercher le "path" executable à partir de la
                          // commande et la liste de "path" du programme

int dash_launch(char **args,
                char **path_tokens);  // Exécuter la commande de l'utilisateur

int check_if_redirection(
    char *string);  // Vérifier s'il y a la signe de redirection "<" dans une
                    // chaine de caractères

int dash_redirection(
    char **args, char **path_tokens,
    int number_args);  // Traiter (vérifier si la commande vérifie bien les
                       // conditions, ouvrir le fichier et transmettre son
                       // contenu dans STDIN, ...) avant d'exécuter la commande
                       // contenant redirection de l'utilisateur + Exécuter la
                       // commande

#endif  // DASH_H
