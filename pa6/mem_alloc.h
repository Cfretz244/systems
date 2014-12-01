#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

/*----- Includes without dependencies -----*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "definitions.h"

/*----- Numerical Constants -----*/

#define MAX_MEMORY 5000

/*----- Macro Definitions -----*/

#define malloc(x) allocate(x, __FILE__, __LINE__)
#define free(x) deallocate(x, __FILE__, __LINE__)

/*----- Memory Allocation Functions -----*/

void *allocate(size_t size, const char *file, const int line);
void deallocate(void *ptr, const char *file, const int line);

#endif
