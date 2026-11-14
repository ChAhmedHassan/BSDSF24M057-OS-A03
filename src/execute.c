#include "shell.h"

/* Run cmd1 | cmd2 | ... with optional  < infile  and  > outfile. */
int execute_pipeline(Pipeline *p)
{
    pid_t pids[MAX_CMDS];
    int prev_read = -1;                      /* read end of the previous pipe */

    for (int i = 0; i < p->n; i++) {
        int fd[2] = { -1, -1 };
        if (i < p->n - 1 && pipe(fd) < 0) {
            perror("pipe");
            return 1;
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            return 1;
        }

        if (pids[i] == 0) {                  /* ---------- child ---------- */
            if (prev_read != -1) {           /* stdin  <- previous pipe */
                dup2(prev_read, STDIN_FILENO);
                close(prev_read);
            }
            if (fd[1] != -1) {               /* stdout -> next pipe */
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
            }

            Command *c = &p->cmds[i];
            if (c->infile != NULL) {         /* < file */
                int in = open(c->infile, O_RDONLY);
                if (in < 0) {
                    perror(c->infile);
                    exit(1);
                }
                dup2(in, STDIN_FILENO);
                close(in);
            }
            if (c->outfile != NULL) {        /* > file */
                int out = open(c->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (out < 0) {
                    perror(c->outfile);
                    exit(1);
                }
                dup2(out, STDOUT_FILENO);
                close(out);
            }

            execvp(c->argv[0], c->argv);
            perror(c->argv[0]);
            exit(127);
        }

        /* ---------- parent: close the ends it no longer needs ---------- */
        if (prev_read != -1)
            close(prev_read);
        if (fd[1] != -1) {
            close(fd[1]);
            prev_read = fd[0];
        } else {
            prev_read = -1;
        }
    }

    /* BACKGROUND HANDLING GOES BELOW THIS LINE */

    /* foreground: wait for every process of the pipeline */
    int status = 0;
    for (int i = 0; i < p->n; i++) {
        int st;
        waitpid(pids[i], &st, 0);
        if (i == p->n - 1)
            status = WIFEXITED(st) ? WEXITSTATUS(st) : 1;
    }
    return status;
}

/* tokenize -> parse -> built-in or execute. Returns the exit status. */
int run_line(const char *line)
{
    char *tokens[MAX_TOKENS];
    int ntok = tokenize(line, tokens);
    if (ntok == 0)
        return 0;

    /* EXPANSION GOES BELOW THIS LINE */

    Pipeline p;
    int status;

    if (parse_pipeline(tokens, ntok, &p) < 0) {
        status = 1;
    } else {
        strncpy(p.text, line, MAX_LINE - 1);
        p.text[MAX_LINE - 1] = '\0';
        if (p.n == 1 && handle_builtin(p.cmds[0].argv))
            status = last_status;
        else
            status = execute_pipeline(&p);
    }

    free_tokens(tokens, ntok);
    last_status = status;
    return status;
}
