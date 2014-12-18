#ifndef DEFINITIONS_H
#define DEFINITIONS_H

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
