#include "shell.h"

/* Show the prompt and read one line. Returns a malloc'd string,
   or NULL when the user presses Ctrl+D (end of input). */
char *read_line(const char *prompt)
{
    char buf[MAX_LINE];

    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return NULL;
    buf[strcspn(buf, "\n")] = '\0';
    return strdup(buf);
}
