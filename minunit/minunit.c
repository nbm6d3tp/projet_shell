#define _GNU_SOURCE
#define MAX_BUFFER_SIZE 1024
#include "minunit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dash_test.h"

MU_TEST(test_dash_launch) {
  char *path_tokens[MAX_PATHS];
  for (int i = 0; i < MAX_PATHS; i++) {
    path_tokens[i] = NULL;
  }
  path_tokens[0] = strdup("/bin");

  char *args1[2] = {"ls", NULL};
  mu_assert_int_eq(0, dash_launch(args1, path_tokens));
  char expected_output[] =
      "dash_test.c\ndash_test.h\nfile.txt\nminunit\nminunit.c\nminunit."
      "h\noutput_ls.txt\noutput_redirection.txt\n";
  FILE *file = fopen("output_ls.txt", "r");
  if (file == NULL) {
    mu_fail("Failed to open the file");
    return;
  }
  char buffer[MAX_BUFFER_SIZE];
  char fileContent[MAX_BUFFER_SIZE] = "";
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    strcat(fileContent, buffer);
  }
  mu_assert_string_eq(expected_output, fileContent);
  fclose(file);

  char *args2[3] = {"vdsv", "-l", NULL};
  mu_check(dash_launch(args2, path_tokens) != 0);

  free(path_tokens[0]);
  path_tokens[0] = NULL;
  mu_check(dash_launch(args1, path_tokens) != 0);

  int counter = 0;
  while (path_tokens[counter] != NULL) {
    free(path_tokens[counter]);
    counter++;
  }
}

MU_TEST(test_find_execute) {
  char *path_tokens[MAX_PATHS];
  char *result;
  for (int i = 0; i < MAX_PATHS; i++) {
    path_tokens[i] = NULL;
  }
  path_tokens[0] = strdup("/bin");
  char *args1[3] = {"ls", "-l", NULL};
  result = find_executable(args1[0], path_tokens);
  mu_assert_string_eq("/bin/ls", result);
  free(result);

  char *args2[3] = {"vsdvds", "-l", NULL};
  result = find_executable(args2[0], path_tokens);
  mu_check(result == NULL);

  free(path_tokens[0]);
  path_tokens[0] = NULL;
  result = find_executable(args1[0], path_tokens);
  mu_check(result == NULL);

  int counter = 0;
  while (path_tokens[counter] != NULL) {
    free(path_tokens[counter]);
    counter++;
  }
}

MU_TEST(test_dash_redirection) {
  char *line1 = "ls -l";
  char *line2 = "wc -l<file.txt";
  mu_check(check_if_redirection(line1) == -1);
  mu_check(check_if_redirection(line2) != -1);

  char *path_tokens[MAX_PATHS];
  for (int i = 0; i < MAX_PATHS; i++) {
    path_tokens[i] = NULL;
  }
  path_tokens[0] = strdup("/bin");
  char *args1[5] = {"wc", "-l", "<", "file.txt", NULL};
  int number_args = 4;
  mu_assert_int_eq(0, dash_redirection(args1, path_tokens, number_args));
  char expected_output[] = "4\n";
  FILE *file = fopen("output_redirection.txt", "r");
  if (file == NULL) {
    mu_fail("Failed to open the file");
    return;
  }
  char buffer[MAX_BUFFER_SIZE];
  if (fgets(buffer, sizeof(buffer), file) == NULL) {
    mu_fail("Failed to read the file");
    fclose(file);
    return;
  }
  mu_assert_string_eq(expected_output, buffer);
  fclose(file);

  char *args2[6] = {"wc", "-l", "<", "file.txt", "hello", NULL};
  mu_assert_int_eq(1, dash_redirection(args2, path_tokens, number_args));

  int counter = 0;
  while (path_tokens[counter] != NULL) {
    free(path_tokens[counter]);
    counter++;
  }
}

MU_TEST(test_dash_split_line) {
  int free_up_counter = 0;
  char line1[] = "ls -l";
  char line2[] = "wc -l<file.txt";
  char token_containing_redirection[] = "<abc<def<<ghi<<<jk<";
  int number_args;
  int has_redirection = 0;

  char **tokens1 = dash_split_line(line1, &number_args, &has_redirection);
  mu_assert_int_eq(2, number_args);
  mu_assert_int_eq(0, has_redirection);
  mu_assert_string_eq("ls", tokens1[0]);
  mu_assert_string_eq("-l", tokens1[1]);
  mu_check(tokens1[2] == NULL);
  free_up_counter = 0;
  while (tokens1[free_up_counter] != NULL) {
    free(tokens1[free_up_counter]);
    free_up_counter++;
  }
  free(tokens1[free_up_counter]);
  free(tokens1);

  char **tokens2 = dash_split_line(line2, &number_args, &has_redirection);
  mu_assert_int_eq(4, number_args);
  mu_assert_int_eq(1, has_redirection);
  mu_assert_string_eq("wc", tokens2[0]);
  mu_assert_string_eq("-l", tokens2[1]);
  mu_assert_string_eq("<", tokens2[2]);
  mu_assert_string_eq("file.txt", tokens2[3]);
  mu_check(tokens2[4] == NULL);
  free_up_counter = 0;
  while (tokens2[free_up_counter] != NULL) {
    free(tokens2[free_up_counter]);
    free_up_counter++;
  }
  free(tokens2[free_up_counter]);
  free(tokens2);

  char **elements = dash_split_redirection_token(token_containing_redirection);
  mu_assert_string_eq("<", elements[0]);
  mu_assert_string_eq("abc", elements[1]);
  mu_assert_string_eq("<", elements[2]);
  mu_assert_string_eq("def", elements[3]);
  mu_assert_string_eq("<", elements[4]);
  mu_assert_string_eq("ghi", elements[5]);
  mu_assert_string_eq("<", elements[6]);
  mu_assert_string_eq("jk", elements[7]);
  mu_assert_string_eq("<", elements[8]);
  mu_check(elements[9] == NULL);

  free_up_counter = 0;
  while (elements[free_up_counter] != NULL) {
    free(elements[free_up_counter]);
    free_up_counter++;
  }
  free(elements);
}

MU_TEST(test_dash_list_paths) {
  char *path_tokens[MAX_PATHS];
  for (int i = 0; i < MAX_PATHS; i++) {
    path_tokens[i] = NULL;
  }
  path_tokens[0] = strdup("/bin");

  char *result1;
  char *args1[3] = {"$PATH", "hello", NULL};
  mu_assert_int_eq(1, dash_list_paths(args1, path_tokens, &result1));

  char *result2;
  char *args2[2] = {"$PATH", NULL};
  mu_assert_int_eq(0, dash_list_paths(args2, path_tokens, &result2));
  mu_assert_string_eq("dash: /bin\n", result2);
  free(result2);

  char *result3;
  path_tokens[1] = strdup("/usr/bin");
  char *args3[2] = {"$PATH", NULL};
  mu_assert_int_eq(0, dash_list_paths(args3, path_tokens, &result3));
  mu_assert_string_eq("dash: /bin:/usr/bin\n", result3);
  free(result3);

  char *result4;
  free(path_tokens[0]);
  path_tokens[0] = NULL;
  free(path_tokens[1]);
  path_tokens[1] = NULL;
  char *args4[2] = {"$PATH", NULL};
  mu_assert_int_eq(0, dash_list_paths(args4, path_tokens, &result4));
  mu_assert_string_eq("dash: \n", result4);
  free(result4);
}

MU_TEST(test_dash_path) {
  char *path_tokens[MAX_PATHS];
  for (int i = 0; i < MAX_PATHS; i++) {
    path_tokens[i] = NULL;
  }
  path_tokens[0] = strdup("/bin");
  char *args1[4] = {"path", "/bin", "/usr/bin", NULL};
  mu_assert_string_eq("/bin", path_tokens[0]);
  dash_path(args1, &path_tokens);
  mu_assert_string_eq("/bin", path_tokens[0]);
  mu_assert_string_eq("/usr/bin", path_tokens[1]);
  mu_check(path_tokens[2] == NULL);

  char *args2[2] = {"path", NULL};
  dash_path(args2, &path_tokens);
  mu_check(path_tokens[0] == NULL);

  int counter = 0;
  while (path_tokens[counter] != NULL) {
    free(path_tokens[counter]);
    counter++;
  }
}

MU_TEST_SUITE(test_suite) {
  MU_RUN_TEST(test_dash_launch);
  MU_RUN_TEST(test_find_execute);
  MU_RUN_TEST(test_dash_redirection);
  MU_RUN_TEST(test_dash_split_line);
  MU_RUN_TEST(test_dash_list_paths);
  MU_RUN_TEST(test_dash_path);
}

int main() {
  MU_RUN_SUITE(test_suite);
  MU_REPORT();
  return 0;
}
