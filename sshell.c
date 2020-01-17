#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
        char args[ARG_MAX + 2][CMDLINE_MAX];
        bool output_redir;
        char *filename;
} Command;

void append(char *s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len + 1] = '\0';
}

struct Command *parseCommand(char *cmdStr)
{
        struct Command *command = malloc(sizeof(struct Command));
        command->output_redir = false;
        char cmd[CMDLINE_MAX] = "";
        char *str;
        u_int i = 0, j = 0;
        // bool hit_output_redir = false, hit_pipe = false;

        for (i = 0; i < strlen(cmdStr) + 1; i++)
        {
                if (cmdStr[i] != ' ' && cmdStr[i] != '\0' && cmdStr[i] != '>' && cmdStr[i] != '|')
                {
                        // Normal char
                        char temp[2] = {cmdStr[i], '\0'};
                        str = strcat(cmd, temp);
                }
                else
                {
                        // Hit space, pipe, or endl
                        // if (hit_output_redir)
                        // {
                        //         command->filename = str;
                        //         hit_output_redir = false;
                        //         continue;
                        // }
                        // if (hit_pipe)
                        // {
                        //         continue;
                        // }

                        // if (cmdStr[i] == '>')
                        // {
                        //         command->output_redir = true;
                        //         hit_output_redir = true;
                        // }
                        // else if (cmdStr[i] == '|')
                        // {
                        //         hit_pipe = true;
                        // }
                        // if (cmd[0] != '\0')
                        // {
                        strcpy(command->args[j], str);
                        cmd[0] = '\0';
                        printf("%s\n", command->args[j]);
                        j++;
                        // }
                }
        }
        strcpy(command->args[j], "\0");
        command->prefix = command->args[0];
        // if (command->output_redir)
        // printf("%s\n", command->filename);

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
