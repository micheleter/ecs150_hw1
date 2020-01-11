#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMDLINE_MAX 512

void print_completion(char cmd[], int retval)
{
    printf("+ completed '%s' [%d]", cmd, retval);
    fflush(stdout);
}

int main(void)
{
    char cmd[CMDLINE_MAX];

    while (1)
    {
        char *nl;
        int retval;

        // Print prompt
        printf("sshell$ ");
        fflush(stdout);

        // Get command line
        fgets(cmd, CMDLINE_MAX, stdin);

        // Print command line if stdin is not provided by terminal
        if (!isatty(STDIN_FILENO))
        {
            printf("%s", cmd);
            fflush(stdout);
        }

        // Remove trailing newline from command line
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';

        // Builtin command
        if (!strcmp(cmd, "exit"))
        {
            retval = system(cmd);
            fprintf(stderr, "Bye...\n");
            print_completion(cmd, retval);
            break;
        }

        // Regular command
        retval = system(cmd);
        fprintf(stdout, "Return status for '%s': %d\n", cmd, retval);
    }

    return EXIT_SUCCESS;
}