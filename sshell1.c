#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512

void print_completion(char cmd[], int retval)
{
    // Print completion message after cmd is executed
    fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);
}

int main(void)
{
    char cmd[CMDLINE_MAX];

    while (1)
    {
        char *nl;
        // int retval;
        // char *fxn;

        // Print prompt
        printf("sshell$ ");
        fflush(stdout);

        // Get command line
        fgets(cmd, CMDLINE_MAX, stdin);

        // Sets 'fxn' equal to first input, if cmd is more than one arg
        // fxn = calloc(strlen(cmd) + 1, sizeof(char));
        // strcpy(fxn, cmd);
        // strtok(fxn, " ");

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

        // Builtin 'exit' command
        if (!strcmp(cmd, "exit"))
        {
            pid_t pid;
            int status;

            pid = fork();
            if (pid == 0)
            {
                // Child
                execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
            }
            else if (pid > 0)
            {
                // Parent
                wait(&status);
                fprintf(stderr, "Bye...\n");
                print_completion(cmd, status);
                exit(1);
            }
            else
            {
                // Error
                perror("fork");
                exit(1);
            }
        }

        // Builtin 'pwd' command
        // if (!strcmp(cmd, "pwd"))
        // {
        //     pid_t pid;
        //     int status;

        //     pid = fork();
        //     if (pid == 0)
        //     {
        //         // Child
        //         retval = execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
        //     }
        //     else if (pid > 0)
        //     {
        //         // Parent
        //         wait(&status);
        //         fprintf(stderr, "Bye...\n");
        //         print_completion(cmd, status);
        //     }
        //     else
        //     {
        //         // Error
        //         perror("fork");
        //         exit(1);
        //     }

        //     char *dir;
        //     retval = getcwd(dir, sizeof(dir));
        //     fprintf(stderr, "%s\n", dir);
        //     print_completion(cmd, retval);
        // }

        // Builtin 'cd' command
        // if (!strcmp(fxn, "cd"))
        // {
        //     char *arg = strtok(NULL, " \0\n");
        //     printf("%s", arg);
        //     retval = chdir(arg);
        //     printf("%d", errno);
        //     // print_completion(cmd, retval);
        // }

        // Regular command
        // retval = system(cmd);
        // fprintf(stdout, "Return status for '%s': %d\n", cmd, retval);
    }

    return EXIT_SUCCESS;
}
