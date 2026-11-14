#include "shell.h"

int last_status = 0;      /* exit status of the most recent command */

static void builtin_help(void)
{
    printf("Built-in commands:\n");
    printf("  cd <dir>   change the current directory\n");
    printf("  exit       leave the shell\n");
    printf("  help       show this message\n");
    printf("  jobs       list background jobs\n");
    /* NEW HELP LINES GO BELOW THIS LINE */
}

/* Returns 1 if arglist[0] was a built-in (and has been run), else 0. */
int handle_builtin(char **arglist)
{
    if (arglist[0] == NULL)
        return 0;

    if (strcmp(arglist[0], "exit") == 0) {
        printf("Goodbye!\n");
        exit(0);
    }
    if (strcmp(arglist[0], "cd") == 0) {
        const char *dir = arglist[1] ? arglist[1] : getenv("HOME");
        if (dir == NULL || chdir(dir) != 0) {
            perror("cd");
            last_status = 1;
        } else {
            last_status = 0;
        }
        return 1;
    }
    if (strcmp(arglist[0], "help") == 0) {
        builtin_help();
        last_status = 0;
        return 1;
    }
    if (strcmp(arglist[0], "jobs") == 0) {
        print_jobs();
        last_status = 0;
        return 1;
    }
    /* NEW BUILT-INS GO BELOW THIS LINE */
    return 0;
}
