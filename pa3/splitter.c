#include "splitter.h"

char *get_line(FILE *file) {
    if (feof(file)) {
        return NULL;
    }
    int size = 1;
    char *input = (char *) malloc(sizeof(char) * size);
    if (input) {
        for (int i = 0; true; i++) {
            char c = fgetc(file);
            if (c == '\n' || c == EOF) {
                input[i] = '\0';
                break;
            }
            input[i] = c;
            if (i == size - 1) {
                size *= 2;
                input = (char *) realloc(input, size);
                if (!input) {
                    errno = 12;
                    return NULL;
                }
            }
        }
        errno = 0;
        return input;
    } else {
        errno = 12;
        return NULL;
    }
}

splitter *create_splitter(FILE *file) {
    splitter *split = (splitter *) malloc(sizeof(splitter));

    if (split) {
        split->file = file;
        if (split->file) {
            split->current_line = get_line(split->file);
            split->remaining = split->current_line;
        } else {
            free(split);
            split = NULL;
        }
    }

    return split;
}

bool is_delimeter(char del) {
    return !(del >= 'A' && del <= 'Z' || del >= 'a' && del <= 'z' || del >= '0' && del <= '9');
}

char *split_next(splitter *split) {
    if (!strlen(split->remaining)) {
        free(split->current_line);
        split->current_line = get_line(split->file);
        split->remaining = split->current_line;
        if (!split->current_line) {
            return NULL;
        }
    }
    char *current = split->remaining;
    while(!is_delimeter(*current) && *current != '\0') {
        current++;
    }
    int size = (current - split->remaining);
    char *token = (char *) malloc(sizeof(char) * (size + 1));
    strncpy(token, split->remaining, size);
    token[size] = '\0';
    while (is_delimeter(*current) && *current != '\0') {
        current++;
    }
    split->remaining = current;
    return token;
}

void destroy_splitter(splitter *split) {
    free(split->current_line);
    fclose(split->file);
    free(split);
}
