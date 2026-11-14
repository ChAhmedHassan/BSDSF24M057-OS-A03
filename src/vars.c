#include "shell.h"

/* Variables are stored in a singly linked list of name/value pairs. */
typedef struct Var {
    char *name;
    char *value;
    struct Var *next;
} Var;

static Var *head = NULL;

void set_var(const char *name, const char *value)
{
    for (Var *v = head; v != NULL; v = v->next) {
        if (strcmp(v->name, name) == 0) {        /* already exists: update */
            free(v->value);
            v->value = strdup(value);
            return;
        }
    }
    Var *v = malloc(sizeof(Var));                /* new variable at the front */
    v->name = strdup(name);
    v->value = strdup(value);
    v->next = head;
    head = v;
}

const char *get_var(const char *name)
{
    for (Var *v = head; v != NULL; v = v->next)
        if (strcmp(v->name, name) == 0)
            return v->value;
    return NULL;
}

void print_vars(void)
{
    for (Var *v = head; v != NULL; v = v->next)
        printf("%s=%s\n", v->name, v->value);
}

/* Is this word of the form NAME=value ? */
int is_assignment(const char *s)
{
    if (!(isalpha((unsigned char)s[0]) || s[0] == '_'))
        return 0;
    int i = 1;
    while (isalnum((unsigned char)s[i]) || s[i] == '_')
        i++;
    return s[i] == '=';
}

/* Replace every word that starts with '$' by its value.
   $? gives the last exit status; unknown names become "". */
void expand_variables(char **tokens, int ntok)
{
    for (int i = 0; i < ntok; i++) {
        if (tokens[i][0] != '$' || tokens[i][1] == '\0')
            continue;

        char *expanded;
        if (strcmp(tokens[i], "$?") == 0) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d", last_status);
            expanded = strdup(buf);
        } else {
            const char *val = get_var(tokens[i] + 1);
            if (val == NULL)
                val = getenv(tokens[i] + 1);     /* e.g. $HOME */
            expanded = strdup(val ? val : "");
        }
        free(tokens[i]);                         /* old word out, new one in */
        tokens[i] = expanded;
    }
}
