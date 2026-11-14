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
int execute(char **arglist);

/* jobs.c */
void print_jobs(void);

/* NEW PROTOTYPES GO BELOW THIS LINE */

/* history.c */
void add_to_history(const char *line);
void print_history(void);
const char *get_history(int n);

#endif
