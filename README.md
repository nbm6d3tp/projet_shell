[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/HdkhcjpH)

**NGUYEN Binh Minh**

**Système d'exploitation: Ubuntu**

# Pour compiler et lancer le programme
- À le root du projet, lancer cette commande pour compiler le programme:
```
gcc -o dash dash.c dash_builtin.c dash_execute.c dash_launch.c dash_read_line.c dash_split_line.c dash_redirection.c -Wall -Wextra -Werror -std=c17
```
- Après, lancer cette commande pour lancer l'application:
```
./dash
```
- Pour tester le "memory leak", lancer cette commande:
```
valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all ./dash
```
# Pour compiler et lancer les testes
- Entrer dans le dossier minunit:
```
cd minunit
```
- Lancer cette commande pour compiler le programme:
```
gcc -o minunit minunit.c dash_test.c
```
- Après, lancer cette commande pour lancer l'application:
```
./minunit
```
- Pour tester le "memory leak", lancer cette commande:
```
valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all ./minunit
```

