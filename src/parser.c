#include "shell.h"

/* Split a line into words. Whitespace separates words; text inside
   "double" or 'single' quotes stays together (the quotes are removed). */
int tokenize(const char *line, char **tokens)
{
    int n = 0;
    const char *p = line;

    while (*p != '\0' && n < MAX_TOKENS - 1) {
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') break;

        char word[MAX_LINE];
        int len = 0;
        char quote = 0;
        while (*p != '\0' && (quote || !isspace((unsigned char)*p))) {
            if (quote) {
                if (*p == quote) quote = 0;
                else if (len < MAX_LINE - 1) word[len++] = *p;
            } else if (*p == '"' || *p == '\'') {
                quote = *p;
            } else if (len < MAX_LINE - 1) {
                word[len++] = *p;
            }
            p++;
        }
        word[len] = '\0';
        tokens[n++] = strdup(word);
    }
    tokens[n] = NULL;
    return n;
}

void free_tokens(char **tokens, int n)
{
    for (int i = 0; i < n; i++)
        free(tokens[i]);
}
