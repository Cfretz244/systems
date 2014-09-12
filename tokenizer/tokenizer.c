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

char proper_char(char current) {
    switch (current) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case 'b':
            return '\b';
        case 'r':
            return '\r';
        case 'f':
            return '\f';
        case 'a':
            return '\a';
        case '\\':
            return '\\';
        case '"':
            return '\"';
        default:
            return '\xFF';
    }
}

char *escape(char *orig, int size) {
    int total_size = size;
    char *orig_ptr = orig;

    // Calculate total length of string after making replacements.
    for (int i = 0; i < size; i++) {
        if (*orig_ptr == '\n' || *orig_ptr == '\t' || *orig_ptr == '\v' || *orig_ptr == '\b' || *orig_ptr == '\r' || *orig_ptr == '\f' ||
            *orig_ptr == '\a' || *orig_ptr == '\\' || *orig_ptr == '\"' || *orig_ptr == '\xFF') {
            if (*orig_ptr == '\xFF') {
                total_size--;
            } else {
                total_size += rep_len - 1;
            }
        }
        orig_ptr++;
    }

    char *expanded = (char *) malloc(sizeof(char) * total_size);
    char *dest = expanded;
    orig_ptr = orig;

    for (int i = 0; i < size; i++) {
        if (*orig_ptr == '\n' || *orig_ptr == '\t' || *orig_ptr == '\v' || *orig_ptr == '\b' || *orig_ptr == '\r' || *orig_ptr == '\f' ||
            *orig_ptr == '\a' || *orig_ptr == '\\' || *orig_ptr == '\"' || *orig_ptr == '\xFF') {
            int offset = orig_ptr - orig;

            if (offset > 0) {
                memcpy(dest, orig, offset);
                orig += offset;
                dest += offset;
            }

            if (*orig_ptr != '\xFF') {
                switch (*orig_ptr) {
                    case '\n':
                        memcpy(dest, newline, rep_len);
                        break;
                    case '\t':
                        memcpy(dest, tab, rep_len);
                        break;
                    case '\v':
                        memcpy(dest, vert_tab, rep_len);
                        break;
                    case '\b':
                        memcpy(dest, backspace, rep_len);
                        break;
                    case '\r':
                        memcpy(dest, car_ret, rep_len);
                        break;
                    case '\f':
                        memcpy(dest, form_feed, rep_len);
                        break;
                    case '\a':
                        memcpy(dest, alert, rep_len);
                        break;
                    case '\\':
                        memcpy(dest, backslash, rep_len);
                        break;
                    case '\"':
                        memcpy(dest, quote, rep_len);
                        break;
                }
                dest += rep_len;
                orig++;
            } else {
                orig++;
            }
        }
        orig_ptr++;
    }

    if (orig != orig_ptr) {
        int offset = orig_ptr - orig;
        memcpy(dest, orig, offset);
        dest += offset;
    }

    *dest = '\0';

    return expanded;
}

char *unescape(char *orig, int size) {
    // Setup local variables and malloc a new string.
    char *new = (char *) malloc(sizeof(char) * size);
    char *dest = new, *curr = orig;

    // Copy across string, replacing escape sequences as we go.
    while (*curr != '\0') {
        if (*curr == '\\') {
            char escape = proper_char(*(curr + 1));
            int offset = curr - orig;
            if (offset > 0) {
                memcpy(dest, orig, offset);
                orig += offset;
                dest += offset;
            }
            *dest = escape;
            dest++;
            orig += 2;
            curr++;
        }
        curr++;
    }

    // Last little bit.
    if (dest != orig) {
        int offset = curr - orig;
        memcpy(dest, orig, offset);
        dest += offset;
    }

    *dest = '\0';

    return new;
}

char **format(char **argv) {
    // Create local variables and get initial string lengths.
    char *del = argv[1];
    int del_len = strlen(del) + 1;
    char *str = argv[2];
    int str_len = strlen(str) + 1;

    // Calculate formatted lengths.
    while (*del != '\0') {
        if (*del == '\\')  {
            del_len--;
        }
        del++;
    }
    while (*str != '\0') {
        if (*str == '\\') {
            str_len--;
        }
        str++;
    }

    char *format_del = unescape(argv[1], del_len);
    char *format_str = unescape(argv[2], str_len);

    char **formatted = (char **) malloc(sizeof(char *) * 2);
    formatted[0] = format_del;
    formatted[1] = format_str;
    return formatted;
}

int isDelimeter(char *current, char *del) {
    // Iterate over all delimeter characters.
    while (*del != '\0') {
        if (*del == *current) {
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

    // Copy string pointers into struct.
    tokenizer->remaining = ts;
    tokenizer->initial = ts;
    tokenizer->delimeters = separators;

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
                token = escape(tk->remaining, offset);

                // Advance the remaining pointer to update the struct.
                tk->remaining = str + 1;

                // Return the found token.
                return token;
            } else {
                // Advance the remaining pointer to skip over worthless token if the size check fails.
                tk->remaining = str + 1;
            }
        }

        // Consider the next character.
        str++;
    }

    // No delimeter was found. Check if string is empty, meaning we're done, or if we still have one last token.
    if (tk->remaining != str) {
        int offset = str - tk->remaining;
        char *token = escape(tk->remaining, offset);
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

    // Bash escapes escape characters when passed inside of double quotes.
    // It's difficult to work with, and this undoes that.
    char **formatted = format(argv);

    // Create tokenizer struct from given arguments and get the first token.
    TokenizerT *tokenizer = TKCreate(formatted[0], formatted[1]);
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
