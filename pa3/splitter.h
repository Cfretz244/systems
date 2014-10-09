#ifndef SPLITTER
#define SPLITTER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definitions.h"

typedef struct splitter {
    FILE *file;
    char *current_line;
    char *remaining;
} splitter;

splitter *create_splitter(char *filename);
char *split_next(splitter *split);
void destroy_splitter(splitter *split);

#endif
