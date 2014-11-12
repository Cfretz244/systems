#ifndef DEFINITIONS
#define DEFINITIONS

#include <stdlib.h>

// A simple boolean typedef from an enum.
typedef enum {
    false,
    true
} bool;

// Enum for distinguishing between query types.
typedef enum {
    AND,
    OR
} query_type;

#endif
