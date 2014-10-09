#include "splitter.h"

char *get_line(FILE *file) {
    if (feof(file)) {
        return NULL;
    }
    char tmp[1048576];
    if (!fgets(tmp, sizeof(tmp), file) || ferror(file)) {
        return NULL;
    }
    char *current_line = (char *) malloc(sizeof(char) * strlen(tmp));
    strcpy(current_line, tmp);
    return current_line;
}

splitter *create_splitter(char *filename) {
    splitter *split = (splitter *) malloc(sizeof(splitter));

    if (split) {
        split->file = fopen(filename, "r");
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
    return !(del >= 'A' && del <= 'Z' || del >= 'a' && del <= 'z');
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
