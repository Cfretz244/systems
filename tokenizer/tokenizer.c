#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TokenizerT_ {
    char *initial;
    char *remaining;
    char *delimeters;
} TokenizerT;

int isDelimeter(char current, char *del) {
    // Iterate over all delimeter characters.
    while (*del != '\0') {
        if (*del == current) {
            return 1;
        }
        del++;
    }

    return 0;
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
        if (isDelimeter(*str, del)) {
            // Figure out how far we've gone and continue only if
            // the generated token would have length > 0.
            int offset = str - tk->remaining;
            if (offset > 1) {
                // Malloc space for token.
                // I know that the size of char is defined to be one.
                // I like being explicit.
                token = (char *) malloc(sizeof(char) * (offset + 1));

                // Copy token and null terminate it.
                memcpy(token, tk->remaining, offset);
                token[offset + 1] = '\0';

                // Advance the remaining pointer to update the struct.
                tk->remaining = str + 1;

                // Return the found token.
                return token;
            } else {
                // Advance the remaining pointer to skip over worthless
                // token if the size check fails.
                tk->remaining = str + 1;
            }
        }

        // Consider the next character.
        str++;
    }

    // No delimeter was found. Check if string is empty, meaning we're
    // done, or if we still have one last token.
    if (*tk->remaining != '\0') {
        char *tmp = tk->remaining;
        tk->remaining = str;
        return tmp;
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

    // Create tokenizer struct from given arguments and
    // get the first token.
    TokenizerT *tokenizer = TKCreate(argv[1], argv[2]);
    char *token = TKGetNextToken(tokenizer);

    // Continuously print the next token until there are
    // none left.
    while (token != NULL) {
        printf("%s\n", token);
        token = TKGetNextToken(tokenizer);
    }

    // Even though the memory will automatically be freed
    // by the termination of the program, it seems good form
    // to explicity free the tokenizer struct.
    TKDestroy(tokenizer);

    return 0;
}
