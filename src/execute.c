#include "shell.h"

/* The fork - exec - wait cycle */
int execute(char **arglist)
{
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {                      /* child: become the new program */
        execvp(arglist[0], arglist);
        perror(arglist[0]);              /* only reached if exec failed */
        exit(127);
    }

    int status;                          /* parent: wait for the child */
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}
