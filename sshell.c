#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CMDLINE_MAX 512
#define ARG_MAX 16

struct Command
{
        char *prefix;
        char *args[ARG_MAX + 2];
        // char *fileName;
} Command;

// char **setPrefix(struct Command *command, char *cmdStr)
// {
//         char **leftOff;
//         command->prefix = strtok_r(cmdStr, " ", leftOff);
//         return leftOff;
// }

struct Command *parseCommand(char *cmdStr)
{
        struct Command *command = malloc(sizeof(struct Command));

        for (u_int i = 0, j = 0; i < strlen(cmdStr); i++)
        {
                if (cmdStr[i] != ' ')
                {
                        // Either a meta-char or normal char
                        if (cmdStr[i] == '>')
                        {
                                // if (cmdStr[i + 1] == '&')
                                // {
                                // }
                        }
                        else if (cmdStr[i] == '|')
                        {
                                //                 // if (cmdStr[i + 1] == '&')
                                //                 // {
                                //                 // }
                        }
                        else
                        {
                                // Normal char
                                printf("Normal\n");
                                char *tmp = strcat(command->args[j], &cmdStr[i]);
                                // command->args[j] = "testing";
                                printf("%s\n", tmp);
                        }
                }
                else
                {
                        // Space found
                        printf("Space\n");
                        j += 1;
                }
        }

        return command;
}

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
                char fxn[CMDLINE_MAX];
                // int retval;
                // int status;
                // pid_t pid;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);
                struct Command *command;

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

                /* Create copy of 'cmd' */
                strcpy(fxn, cmd);
                command = parseCommand(fxn);
                command = command;

                /* Builtin commands */
                //         if (!strcmp(command->prefix, "exit"))
                //         {
                //                 fprintf(stderr, "Bye...\n");
                //                 print_completion(cmd, errno);
                //                 break;
                //         }
                //         else if (!strcmp(cmd, "pwd"))
                //         {
                //                 char buf[CMDLINE_MAX];
                //                 char *dir = getcwd(buf, (size_t)CMDLINE_MAX);
                //                 if (dir)
                //                 {
                //                         printf("%s\n", dir);
                //                         print_completion(cmd, errno);
                //                 }
                //                 else
                //                 {
                //                         perror("getcwd");
                //                 }
                //         }
                //         else if (!strcmp(command->prefix, "cd"))
                //         {
                //                 if (command->args[1] != NULL)
                //                 {
                //                         retval = chdir(command->args[1]);
                //                         if (!retval)
                //                         {
                //                                 print_completion(cmd, retval);
                //                         }
                //                         else
                //                         {
                //                                 perror("chdir");
                //                         }
                //                 }
                //                 else
                //                 {
                //                         fprintf(stderr, "Null directory\n");
                //                 }
                //         }
                //         else
                //         {
                //                 /* Regular command */
                //                 pid = fork();
                //                 if (pid == 0)
                //                 {
                //                         // Child
                //                         execvp(command->prefix, command->args);
                //                         perror("execvp");
                //                         exit(1);
                //                 }
                //                 else if (pid > 0)
                //                 {
                //                         // Parent
                //                         waitpid(-1, &status, 0);
                //                         print_completion(cmd, status);
                //                 }
                //                 else
                //                 {
                //                         // Error
                //                         perror("fork");
                //                         exit(1);
                //                 }
                //         }
        }

        return EXIT_SUCCESS;
}
