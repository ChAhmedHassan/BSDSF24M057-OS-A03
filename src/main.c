#include "shell.h"

int main(void)
{
    char *line;
    char *tokens[MAX_TOKENS];

    while (1) {
        line = read_line(PROMPT);
        if (line == NULL) {              /* Ctrl+D */
            printf("\n");
            break;
        }

        int n = tokenize(line, tokens);
        if (n > 0) {
            if (!handle_builtin(tokens))     /* built-in? run inside the shell */
                execute(tokens);             /* otherwise fork-exec-wait */
        }
        free_tokens(tokens, n);
        free(line);
    }
    return 0;
}
