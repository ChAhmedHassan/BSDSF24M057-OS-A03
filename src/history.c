#include "shell.h"

static char *history[HISTORY_SIZE];
static int hist_count = 0;

/* Store a command. When the array is full the oldest one is dropped. */
void add_to_history(const char *line)
{
    if (hist_count == HISTORY_SIZE) {
        free(history[0]);
        memmove(&history[0], &history[1], (HISTORY_SIZE - 1) * sizeof(char *));
        hist_count--;
    }
    history[hist_count++] = strdup(line);
}

void print_history(void)
{
    for (int i = 0; i < hist_count; i++)
        printf("%4d  %s\n", i + 1, history[i]);
}

/* n is 1-based. Returns NULL if n is out of range. */
const char *get_history(int n)
{
    if (n < 1 || n > hist_count)
        return NULL;
    return history[n - 1];
}
