#include "shell.h"

#define MAX_BLOCK 64

/* Copy of s without leading/trailing spaces. */
static char *trim_copy(const char *s)
{
    while (isspace((unsigned char)*s)) s++;
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) len--;
    char *out = malloc(len + 1);
    memcpy(out, s, len);
    out[len] = '\0';
    return out;
}

/* Is the first word of the line exactly "if" ? */
int is_if_command(const char *line)
{
    while (isspace((unsigned char)*line)) line++;
    return strncmp(line, "if", 2) == 0 &&
           (line[2] == '\0' || isspace((unsigned char)line[2]));
}

/* first_line is the line that started with "if". Keep reading lines until
   "fi", store the then-part and else-part, run the condition, then run
   the matching part. */
int handle_if(const char *first_line)
{
    char *then_cmds[MAX_BLOCK];
    char *else_cmds[MAX_BLOCK];
    int nthen = 0, nelse = 0;
    int state = 0;          /* 0: waiting for "then", 1: then-part, 2: else-part */
    int closed = 0;
    int status = 1;

    while (isspace((unsigned char)*first_line)) first_line++;
    char *cond = trim_copy(first_line + 2);          /* text after the word "if" */

    while (!closed) {
        char *raw = read_line("> ");                 /* continuation prompt */
        if (raw == NULL)
            break;                                   /* Ctrl+D */
        char *line = trim_copy(raw);
        free(raw);

        if (state == 0 && strcmp(line, "then") == 0) {
            state = 1;
            free(line);
        } else if (state == 1 && strcmp(line, "else") == 0) {
            state = 2;
            free(line);
        } else if (strcmp(line, "fi") == 0) {
            closed = 1;
            free(line);
        } else if (state == 1 && nthen < MAX_BLOCK) {
            then_cmds[nthen++] = line;
        } else if (state == 2 && nelse < MAX_BLOCK) {
            else_cmds[nelse++] = line;
        } else {
            free(line);
        }
    }

    if (!closed || state == 0 || cond[0] == '\0') {
        fprintf(stderr, "myshell: syntax error in if statement\n");
    } else {
        status = process_line(cond);                 /* exit status of the condition */
        char **block = (status == 0) ? then_cmds : else_cmds;
        int count = (status == 0) ? nthen : nelse;
        for (int i = 0; i < count; i++)
            process_line(block[i]);
    }

    for (int i = 0; i < nthen; i++) free(then_cmds[i]);
    for (int i = 0; i < nelse; i++) free(else_cmds[i]);
    free(cond);
    return status;
}
