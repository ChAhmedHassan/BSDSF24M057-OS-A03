#include "shell.h"

#define MAX_JOBS 64

typedef struct {
    int   id;
    pid_t pid;
    char  cmd[MAX_LINE];
    int   active;
} Job;

static Job jobs[MAX_JOBS];
static int next_id = 1;

/* Remember a background process. */
void add_job(pid_t pid, const char *cmd)
{
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!jobs[i].active) {
            jobs[i].active = 1;
            jobs[i].id = next_id++;
            jobs[i].pid = pid;
            strncpy(jobs[i].cmd, cmd, MAX_LINE - 1);
            jobs[i].cmd[MAX_LINE - 1] = '\0';
            printf("[%d] %d\n", jobs[i].id, (int)pid);
            return;
        }
    }
    fprintf(stderr, "myshell: too many background jobs\n");
}

/* Collect finished children so they do not stay as zombies.
   WNOHANG makes waitpid return immediately if nobody has finished. */
void reap_background(void)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (jobs[i].active && jobs[i].pid == pid) {
                printf("[%d] Done    %s\n", jobs[i].id, jobs[i].cmd);
                jobs[i].active = 0;
                break;
            }
        }
    }
}

void print_jobs(void)
{
    int found = 0;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].active) {
            printf("[%d] %d Running  %s\n", jobs[i].id, (int)jobs[i].pid, jobs[i].cmd);
            found = 1;
        }
    }
    if (!found)
        printf("No background jobs.\n");
}
