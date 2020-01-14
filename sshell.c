#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define CMDLINE_MAX 512
#define ARG_MAX 16

// struct Command
// {
//         char *prefix;
//         char *args[ARG_MAX + 1];
// };

// struct Command *parseCommand(char *cmdStr)
// {
//         struct Command *command = malloc(sizeof(*command));
//         int i = 1;

//         char *cur = strtok(cmdStr, " ");
//         (command->args)[0] = cur;
//         command->prefix = cur;

//         while (cur)
//         {
//                 cur = strtok(NULL, " ");
//                 (command->args)[i] = cur;
//                 i++;
//         }
//         (command->args)[i] = '\0';

//         return command;
// }

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
                // char fxn[CMDLINE_MAX];
                // int retval;
                int status;
                pid_t pid;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);
                // strcpy(fxn, cmd);
                // struct Command *command = parseCommand(fxn);
                char *args[2] = {cmd, NULL};

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO))
                {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';

                /* Builtin command */
                if (!strcmp(cmd, "exit"))
                {
                        fprintf(stderr, "Bye...\n");
                        print_completion(cmd, errno);
                        break;
                }

                /* Regular command */
                pid = fork();
                if (pid == 0)
                {
                        // Child
                        execvp(args[0], args);
                        exit(1);
                }
                else if (pid > 0)
                {
                        // Parent
                        waitpid(-1, &status, 0);
                        print_completion(cmd, status);
                }
                else
                {
                        // Error
                        perror("fork");
                        exit(1);
                }
        }

        return EXIT_SUCCESS;
}
