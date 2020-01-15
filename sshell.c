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

struct Command *parseCommand(char *cmdStr)
{
        struct Command *command = malloc(sizeof(struct Command));
        // char* nr;

        int i = 1;
        char *cur = strtok(cmdStr, " ");
        command->prefix = cur;
        command->args[0] = command->prefix;

        while (cur)
        {
                cur = strtok(NULL, " ");

                command->args[i] = cur;

                i++;
        }
        command->args[i] = NULL;

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
                char *nr;
                char *cur;
                // char *tmp;
                char fxn[CMDLINE_MAX];
                int retval;
                int status;
                // int fd;
                pid_t pid;
                // bool needsRedirection = false;

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

                /* Finds output redirection character */
                strcpy(fxn, cmd);
                nr = strchr(fxn, '>');
                // int count = 1;
                if (nr) {
                        cur = strtok(fxn, ">");
                        // printf("%s\n", cur);
                        while (cur) {
                          cur = strtok(NULL, ">");
                        }

                }
                else {
                  command = parseCommand(fxn);
                }



                /* Builtin commands */
                if (!strcmp(command->prefix, "exit"))
                {
                        fprintf(stderr, "Bye...\n");
                        print_completion(cmd, errno);
                        break;
                }
                else if (!strcmp(cmd, "pwd"))
                {
                        char buf[CMDLINE_MAX];
                        char *dir = getcwd(buf, (size_t)CMDLINE_MAX);
                        if (dir)
                        {
                                printf("%s\n", dir);
                                print_completion(cmd, errno);
                        }
                        else
                        {
                                perror("getcwd");
                        }
                }
                else if (!strcmp(command->prefix, "cd"))
                {
                  			if (command->args[1] != NULL)
                  			{
                                retval = chdir(command->args[1]);
                                if (!retval)
                                {
                                       print_completion(cmd, retval);
                                }
                  				      else
                  				      {
                  					           perror("chdir");
                  				      }
                  			}
                  			else
                  			{
                  				fprintf(stderr, "Null directory\n");
                  			}
                }
                else
                {
                        /* Regular command */
                        pid = fork();
                        if (pid == 0)
                        {
                                // Child
                                execvp(command->prefix, command->args);
                                perror("execvp");
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
        }

        return EXIT_SUCCESS;
}
