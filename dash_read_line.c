#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include "dash.h"

char *dash_read_line() {
  char *line = NULL;
  size_t len = 0;
  getline(&line, &len, stdin);
  return line;
}
