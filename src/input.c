#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>

/* Names offered when the user presses Tab on the first word.
   ("set" is added in Feature 8.) */
static const char *builtin_names[] = {
    "cd", "exit", "help", "jobs", "history", "set", NULL
};

/* Called by Readline repeatedly; returns one matching name each time. */
static char *builtin_generator(const char *text, int state)
{
    static int index, len;

    if (state == 0) {
        index = 0;
        len = strlen(text);
    }
    while (builtin_names[index] != NULL) {
        const char *name = builtin_names[index++];
        if (strncmp(name, text, len) == 0)
            return strdup(name);
    }
    return NULL;
}

/* First word on the line: try built-in names.
   Otherwise (or if nothing matched) return NULL so Readline falls back
   to its default completion of file names in the current directory. */
static char **shell_completion(const char *text, int start, int end)
{
    (void)end;
    if (start == 0)
        return rl_completion_matches(text, builtin_generator);
    return NULL;
}

char *read_line(const char *prompt)
{
    static int initialized = 0;

    if (!initialized) {
        rl_attempted_completion_function = shell_completion;
        initialized = 1;
    }
    return readline(prompt);     /* malloc'd; NULL on Ctrl+D */
}
