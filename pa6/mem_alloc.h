#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

/*----- Includes without dependencies -----*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "definitions.h"

/*----- Numerical Constants -----*/

#define MIN_ALLOC 2
#define MAX_MEMORY 5000
#define SINGLE_BYTE_MAX 255
#define DOUBLE_BYTE_MAX 65535
#define QUAD_BYTE_MAX 4294967295
#define SPLIT_THRESHOLD 0.01

/*----- Macro Definitions -----*/

#define malloc(x) allocate(x, __FILE__, __LINE__)
#define free(x) deallocate(x, __FILE__, __LINE__)

/*----- Memory Allocation Functions -----*/

void *allocate(size_t size, const char *file, const int line);
void deallocate(void *ptr, const char *file, const int line);

#endif
