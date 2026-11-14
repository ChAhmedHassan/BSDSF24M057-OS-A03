#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROMPT     "myshell> "
#define MAX_LINE   1024
#define MAX_TOKENS 128
#define HISTORY_SIZE 20

extern int last_status;

/* input.c */
char *read_line(const char *prompt);

/* parser.c */
int tokenize(const char *line, char **tokens);
void free_tokens(char **tokens, int n);

/* shell.c */
int handle_builtin(char **arglist);

/* execute.c */

/* jobs.c */
void print_jobs(void);

/* NEW PROTOTYPES GO BELOW THIS LINE */

/* jobs.c / chain.c */
void add_job(pid_t pid, const char *cmd);
void reap_background(void);
int process_line(const char *line);

/* parser.c / execute.c: pipelines */
#define MAX_ARGS 64
#define MAX_CMDS 16

typedef struct {
    char *argv[MAX_ARGS];
    char *infile;                 /* file after '<', or NULL */
    char *outfile;                /* file after '>', or NULL */
} Command;

typedef struct {
    Command cmds[MAX_CMDS];
    int  n;                       /* number of commands joined by '|' */
    int  background;              /* 1 if the line ended with '&' (Feature 6) */
    char text[MAX_LINE];          /* original text (used by jobs in Feature 6) */
} Pipeline;

int parse_pipeline(char **tokens, int ntok, Pipeline *p);
int execute_pipeline(Pipeline *p);
int run_line(const char *line);

/* history.c */
void add_to_history(const char *line);
void print_history(void);
const char *get_history(int n);

#endif
