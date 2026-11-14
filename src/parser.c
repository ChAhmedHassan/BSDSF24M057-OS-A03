#include "shell.h"

static int is_op(const char *s)
{
    return s[0] != '\0' && s[1] == '\0' && strchr("<>|&", s[0]) != NULL;
}

/* Split a line into tokens. Whitespace separates tokens, quotes group text,
   and the characters  <  >  |  &  become tokens of their own. */
int tokenize(const char *line, char **tokens)
{
    int n = 0;
    const char *p = line;

    while (*p != '\0' && n < MAX_TOKENS - 1) {
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') break;

        if (strchr("<>|&", *p) != NULL) {          /* operator token */
            char op[2] = { *p, '\0' };
            tokens[n++] = strdup(op);
            p++;
            continue;
        }

        char word[MAX_LINE];
        int len = 0;
        char quote = 0;
        while (*p != '\0') {
            if (quote) {
                if (*p == quote) quote = 0;
                else if (len < MAX_LINE - 1) word[len++] = *p;
            } else if (*p == '"' || *p == '\'') {
                quote = *p;
            } else if (isspace((unsigned char)*p) || strchr("<>|&", *p) != NULL) {
                break;
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

/* Turn the token list into a Pipeline. Returns 0 on success, -1 on a syntax error. */
int parse_pipeline(char **tokens, int ntok, Pipeline *p)
{
    memset(p, 0, sizeof(*p));
    p->n = 1;
    int argc = 0;                         /* words collected for the current command */

    for (int i = 0; i < ntok; i++) {
        char *t = tokens[i];
        Command *c = &p->cmds[p->n - 1];

        if (strcmp(t, "|") == 0) {
            if (argc == 0 || p->n >= MAX_CMDS) {
                fprintf(stderr, "myshell: syntax error near '|'\n");
                return -1;
            }
            p->n++;
            argc = 0;
        } else if (strcmp(t, "<") == 0 || strcmp(t, ">") == 0) {
            if (i + 1 >= ntok || is_op(tokens[i + 1])) {
                fprintf(stderr, "myshell: syntax error: file name expected after '%s'\n", t);
                return -1;
            }
            if (t[0] == '<') c->infile  = tokens[++i];
            else             c->outfile = tokens[++i];
        } else if (strcmp(t, "&") == 0) {
            if (i != ntok - 1) {
                fprintf(stderr, "myshell: '&' is only allowed at the end\n");
                return -1;
            }
            p->background = 1;
        } else {
            if (argc >= MAX_ARGS - 1) {
                fprintf(stderr, "myshell: too many arguments\n");
                return -1;
            }
            c->argv[argc++] = t;
        }
    }

    if (argc == 0) {
        fprintf(stderr, "myshell: syntax error: command expected\n");
        return -1;
    }
    return 0;
}
