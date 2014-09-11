#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TokenizerT_ {
    char *initial;
    char *remaining;
    char *delimeters;
} TokenizerT;

const char *newline = "[0x0a]";
const char *tab = "[0x09]";
const char *vert_tab = "[0x0b]";
const char *backspace = "[0x08]";
const char *car_ret = "[0x0d]";
const char *form_feed = "[0x0c]";
const char *alert = "[0x07]";
const char *backslash = "[0x5c]";
const char *quote = "[0x22]";
const int rep_len = 6;

int isDelimeter(char *current, char *del) {
    // Iterate over all delimeter characters.
    while (*del != '\0') {
        if (*del != '\\') {
            if (*del == *current) {
                return 1;
            }
        } else {
            if (*del == *current && *(del + 1) == *(current + 1)) {
                return 1;
            }
            del++;
        }
        del++;
    }

    return 0;
}

char *unescape(char *source, int length) {
    // Make copies of necessary variables.
    int total_length = length;
    char *src_ptr = source;

    // Calculate total length of string after making replacements.
    for (int i = 0; i < length; i++) {
        if (*src_ptr == '\\') {
            char type = *(src_ptr + 1);
            if (type == 'n' || type == 't' || type == 'v' || type == 'b' || type == 'r' || type == 'f' || type == 'a' ||
                    type == '\\' || type == '"') {
                total_length += rep_len - 2;
            } else {
                total_length--;
            }
        }
        src_ptr++;
    }

    // Malloc space for expanded token and setup pointers.
    char *expanded = (char *) malloc(sizeof(char) * total_length);
    src_ptr = source;
    char *dest_ptr = expanded;

    // I hate the code duplication here, but we needed to know the size of the token before we could start working on it.
    for (int i = 0; i < length; i++) {
        if (*src_ptr == '\\') {
            int offset = src_ptr - source;

            // Copy over substring if there is one.
            if (offset > 0) {
                memcpy(dest_ptr, source, offset);
                source = src_ptr;
                dest_ptr += offset;
            }

            // Indentify character case and copy over bracketed hex.
            switch (*(src_ptr + 1)) {
                case 'n':
                    memcpy(dest_ptr, newline, rep_len);
                    break;
                case 't':
                    memcpy(dest_ptr, tab, rep_len);
                    break;
                case 'v':
                    memcpy(dest_ptr, vert_tab, rep_len);
                    break;
                case 'b':
                    memcpy(dest_ptr, backspace, rep_len);
                    break;
                case 'r':
                    memcpy(dest_ptr, car_ret, rep_len);
                    break;
                case 'f':
                    memcpy(dest_ptr, form_feed, rep_len);
                    break;
                case 'a':
                    memcpy(dest_ptr, alert, rep_len);
                    break;
                case '\\':
                    memcpy(dest_ptr, backslash, rep_len);
                    break;
                case '"':
                    memcpy(dest_ptr, quote, rep_len);
                    break;
            }

            // Advance pointers.
            dest_ptr += rep_len;
            source += 2;
        }
        src_ptr++;
    }

    if (source != src_ptr) {
        int offset = src_ptr - source;
        memcpy(dest_ptr, source, offset);
        dest_ptr += offset;
    }

    *dest_ptr = '\0';

    return expanded;
}

TokenizerT *TKCreate(char *separators, char *ts) {
    // Malloc our tokenizer.
    // I know the cast isn't necessary. I like being explicit.
    TokenizerT *tokenizer = (TokenizerT *) malloc(sizeof(TokenizerT));

    // Copy first string into the tokenizer struct.
    int size = strlen(ts) + 1;
    char *copy = (char *) malloc(sizeof(char) * size);
    strcpy(copy, ts);
    tokenizer->remaining = copy;
    tokenizer->initial = copy;

    // Copy second string into the tokenizer struct.
    size = strlen(separators) + 1;
    copy = (char *) malloc(sizeof(char) * size);
    strcpy(copy, separators);
    tokenizer->delimeters = copy;

    // Tokenizer is properly initialized. Time to return it.
    return tokenizer;
}

void TKDestroy(TokenizerT *tk) {
    free(tk->initial);
    free(tk->delimeters);
    free(tk);
}

char *TKGetNextToken(TokenizerT *tk) {
    // Create local variables.
    char *str = tk->remaining;
    char *del = tk->delimeters;
    char *token;

    // Loop over string characters until we find a delimeter.
    while (*str != '\0') {
        if (isDelimeter(str, del)) {
            // Figure out how far we've gone and continue only if the generated token would have length > 0.
            int offset = str - tk->remaining;
            if (offset > 1) {
                // Call unescape to parse out token and replace escape characters.
                token = unescape(tk->remaining, offset);

                // Advance the remaining pointer to update the struct.
                tk->remaining = str + 1;

                // Return the found token.
                return token;
            } else {
                // Advance the remaining pointer to skip over worthless token if the size check fails.
                if (*str == '\\') {
                    tk->remaining = str + 2;
                } else {
                    tk->remaining = str + 1;
                }
            }
        }

        // Consider the next character.
        if (*str == '\\') {
            str += 2;
        } else {
            str++;
        }
    }

    // No delimeter was found. Check if string is empty, meaning we're done, or if we still have one last token.
    if (tk->remaining != str) {
        int offset = str - tk->remaining;
        char *token = unescape(tk->remaining, offset);
        tk->remaining = str;
        return token;
    } else {
        return NULL;
    }
}

int main(int argc, char **argv) {
    // Perform simple argument count check.
    if (argc != 3) {
        fprintf(stderr, "Error: Incorrect number of arguments.\n");
        return 1;
    }

    // Create tokenizer struct from given arguments and get the first token.
    TokenizerT *tokenizer = TKCreate(argv[1], argv[2]);
    char *token = TKGetNextToken(tokenizer);

    // Continuously print the next token until there are none left.
    while (token != NULL) {
        printf("%s\n", token);
        token = TKGetNextToken(tokenizer);
    }

    // Even though the memory will automatically be freed by the termination of the program, it seems good form to explicity
    // free the tokenizer struct.
    TKDestroy(tokenizer);

    return 0;
}
