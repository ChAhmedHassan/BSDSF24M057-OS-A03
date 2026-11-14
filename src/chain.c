#include "shell.h"

/* Split the line at every ';' that is not inside quotes and run the
   parts one after another. Returns the status of the last part. */
int process_line(const char *line)
{
    char *copy = strdup(line);
    char *start = copy;
    char quote = 0;
    int status = 0;

    for (char *p = copy; ; p++) {
        if (quote) {
            if (*p == quote) quote = 0;
        } else if (*p == '"' || *p == '\'') {
            quote = *p;
        }

        if ((*p == ';' && !quote) || *p == '\0') {
            int at_end = (*p == '\0');
            *p = '\0';
            status = run_line(start);        /* runs and waits before the next one */
            if (at_end)
                break;
            start = p + 1;
        }
    }
    free(copy);
    return status;
}
