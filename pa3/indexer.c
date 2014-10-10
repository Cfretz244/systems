#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include "definitions.h"
#include "nodes.h"
#include "hash.h"
#include "splitter.h"

#define OPEN_TAG_LENGTH 7
#define CLOSE_TAG_LENGTH 8

void handle_output(char *path, hash *table);
void handle_file(char *path, hash *table);
void handle_dir(char *path, hash *table);
bool is_file(char *path);
bool is_dir(char *path);
char *get_user_input();
void sort(char **keys, int left, int right);
char *enforce(char *str, int needed, int size, int filled, int *new_size);
void panic(char *reason);
void sanitize(char **argv);

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error: Wrong number of arguments\n");
        return 1;
    }

    sanitize(argv);

    hash *table = create_hash();

    if (!table) {
        panic("Could not allocate memory for hash table");
    }

    if (is_file(argv[2])) {
        handle_file(argv[2], table);
    } else if (is_dir(argv[2])) {
        handle_dir(argv[2], table);
    } else {
        fprintf(stderr, "Error: Could not open specified file. Perhaps it doesn't exist?\n");
        destroy_hash(table);
        return 1;
    }

    bool should_continue = false;
    if (is_file(argv[1])) {
        printf("Warning: The file you requested to save to already exists. Would you like to overwrite?\n");
        char *response = get_user_input();
        while (strncmp(response, "y", 1) && strncmp(response, "n", 1)) {
            printf("Please be a good user and enter either yes or no.\n");
            response = get_user_input();
        }
        if (!strncmp(response, "y", 1)) {
            if (remove(argv[1])) {
                fprintf(stderr, "Error: File could not be removed. Perhaps you don't have the necessary permissions?\n");
            } else {
                should_continue = true;
            }
        } else {
            fprintf(stderr, "Error: Requested output file already exists\n");
        }
        free(response);
    } else if (is_dir(argv[1])) {
        printf("Warning: The file you requested to save to is a directory. Would you like to recursively delete said directory?\n");
        char *response = get_user_input();
        while (strncmp(response, "y", 1) && strncmp(response, "n", 1)) {
            printf("Please be a good user and enter either yes or no\n");
            response = get_user_input();
        }
        if (!strncmp(response, "y", 1)) {
            if (rmdir(argv[1])) {
                fprintf(stderr, "Error: Directory could not be removed. Perhaps you don't have the necessary permissions?\n");
            } else {
                should_continue = true;
            }
        } else {
            fprintf(stderr, "Error: Requested output file is a directory\n");
        }
        free(response);
    } else {
        should_continue = true;
        remove(argv[1]);
    }

    if (should_continue) {
        handle_output(argv[1], table);
    } else {
        destroy_hash(table);
        return 1;
    }

    return 0;
}

void handle_output(char *path, hash *table) {
    FILE *output = fopen(path, "wr");
    char **keys = get_keys(table);
    sort(keys, 0, table->count - 1);

    for (int i = 0; i < table->count; i++) {
        int size = 100, filled = 0, word_count = 0;
        char *line = (char *) malloc(sizeof(char) * size), *key = keys[i];
        int needed = OPEN_TAG_LENGTH + strlen(key) + 1;
        line = enforce(line, needed, size, filled, &size);
        sprintf(line, "<list> %s\n", key);
        filled += needed;
        for (index_node *node = get(table, key); node; node = node->next) {
            word_count++;
            char *filename = node->filename, *format;
            if (word_count && word_count % 5 == 0 || !node->next) {
                needed = strlen(filename);
                format = "%s";
            } else {
                needed = strlen(filename) + 1;
                format = "%s ";
            }
            line = enforce(line, needed, size, filled, &size);
            sprintf(line + filled, format, filename);
            filled += needed;
            if (word_count && word_count % 5 == 0) {
                line = enforce(line, 1, size, filled, &size);
                line[filled] = '\n';
                filled++;
            }
        }
        if (word_count % 5) {
            line = enforce(line, 1 + CLOSE_TAG_LENGTH, size, filled, &size);
            strcat(line + filled, "\n</list>\n");
            filled += CLOSE_TAG_LENGTH + 1;
        } else {
            line = enforce(line, CLOSE_TAG_LENGTH, size, filled, &size);
            strcat(line + filled, "</list>\n");
            filled += CLOSE_TAG_LENGTH;
        }
        line[filled] = '\0';
        if (fputs(line, output) == EOF) {
            panic("Could not write to specified file");
        }
    }

    fclose(output);
}

void handle_dir(char *path, hash *table) {
    DIR *directory = opendir(path);
    if (directory) {
        for (struct dirent *entry = readdir(directory); entry; entry = readdir(directory)) {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                char new_path[strlen(path) + strlen(entry->d_name) + 2];
                sprintf(new_path, "%s/%s", path, entry->d_name);
                if (entry->d_type & DT_REG) {
                    handle_file(new_path, table);
                } else if (entry->d_type & DT_DIR) {
                    handle_dir(new_path, table);
                } else {
                    fprintf(stderr, "Warning: Directory hierarchy contains an entry which is neither a regular file nor directory, skipping it\n");
                }
            }
        }
    } else {
        fprintf(stderr, "Error: Directory %s could not be opened\n", path);
    }
}

void handle_file(char *path, hash *table) {
    FILE *file = fopen(path, "r");
    if (file) {
        splitter *split = create_splitter(file);
        for (char *token = split_next(split); token; token = split_next(split)) {
            index_node *head = get(table, token);
            if (head) {
                index_node *target = find_index_node(head, path);
                if (target) {
                    target->count++;
                } else {
                    target = create_index_node(path);
                    target->count++;
                    head = insert_index_node(head, target);
                    update(table, token, head);
                }
            } else {
                index_node *node = create_index_node(path);
                node->count++;
                put(table, token, node);
            }
        }
        if (errno) {
            panic("Splitter could not allocate memory for string");
        }
    } else {
        fprintf(stderr, "Error: File %s could not be opened\n", path);
    }
}

bool is_file(char *path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

bool is_dir(char *path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

char *get_user_input() {
    int size = 1;
    char *input = (char *) malloc(sizeof(char) * size);
    if (input) {
        while (true) {
            char c = getchar();
            if (c == EOF) {
                break;
            }
            if (!isspace(c)) {
                ungetc(c, stdin);
                break;
            }
        }
        for (int i = 0; true; i++) {
            char c = getchar();
            if (isspace(c) || c == EOF) {
                input[i] = '\0';
                break;
            }
            input[i] = c;
            if (i == size - 1) {
                size *= 2;
                input = (char *) realloc(input, size);
                if (!input) {
                    panic("Could not allocate space for input string");
                }
            }
        }
        return input;
    } else {
        panic("Could not allocate space for input string");
    }
}

void sort(char **keys, int left, int right) {
    if (left < right) {
        char *first = keys[left], *last = keys[right], *mid = keys[(left + right) / 2], *chosen;
        int pivot;
        if (strcmp(first, mid) >= 0 && strcmp(first, last) <= 0) {
            pivot = left;
            chosen = first;
        } else if(strcmp(mid, first) >= 0 && strcmp(mid, last) <= 0) {
            pivot = (left + right) / 2;
            chosen = mid;
        } else {
            pivot = right;
            chosen = last;
        }
        char *tmp = keys[right];
        keys[right] = keys[pivot];
        keys[pivot] = tmp;
        int index = left;
        for (int i = left; i < right; i++) {
            if (strcmp(keys[i], chosen) > 0) {
                tmp = keys[i];
                keys[i] = keys[index];
                keys[index] = tmp;
                index++;
            }
        }
        tmp = keys[index];
        keys[index] = keys[right];
        keys[right] = tmp;
        sort(keys, left, index - 1);
        sort(keys, index + 1, right);
    }
}


char *enforce(char *str, int needed, int size, int filled, int *new_size) {
    while (needed + filled > size - filled) {
        size *= 2;
        str = (char *) realloc(str, size);
        if (!str) {
            panic("Could not allocate memory for string");
        }
    }
    *new_size = size;
    return str;
}

void panic(char *reason) {
    fprintf(stderr, "Fatal Error: Sorry, the indexer has encountered an unrecoverable error. Given reason was: %s\n", reason);
    exit(1);
}

void sanitize(char **argv) {
    if (argv[1][strlen(argv[1]) - 1] == '/') {
        argv[1][strlen(argv[1]) - 1] = '\0';
    }
    if (argv[2][strlen(argv[2]) - 1] == '/') {
        argv[2][strlen(argv[2]) - 1] = '\0';
    }
}
