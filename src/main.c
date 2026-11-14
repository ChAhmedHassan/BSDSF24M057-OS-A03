#include "shell.h"
#include <readline/history.h>

static int is_blank(const char *s)
{
    while (*s) {
        if (!isspace((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

int main(void)
{
    char *line;

    while (1) {
        line = read_line(PROMPT);
        if (line == NULL) {
            printf("\n");
            break;
        }
        if (is_blank(line)) {
            free(line);
            continue;
        }

        if (line[0] == '!') {
            const char *old = get_history(atoi(line + 1));
            if (old == NULL) {
                fprintf(stderr, "myshell: %s: event not found\n", line);
                free(line);
                continue;
            }
            free(line);
            line = strdup(old);
            printf("%s\n", line);
        }

        add_to_history(line);
        add_history(line);

        run_line(line);
        free(line);
    }
    return 0;
}
