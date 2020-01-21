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
#define CMDS_MAX 4

struct Command
{
  char *prefix;
  char *args[ARG_MAX + 2];
  bool needs_output_redir;
  char *filename;
} Command;

char *trim(char *str)
{
  printf("TRIM\n");
  int len = strlen(str);

  if (str[0] == ' ')
  {
    memmove(str, str + 1, len);
  }
  else if (str[len - 1] == ' ')
  {
    str[len - 1] = '\0';
  }
  return str;
}

struct Command *parseCommand(char *cmdStr)
{

  struct Command *command = malloc(sizeof(struct Command));
  command->needs_output_redir = false;
  char cmd[CMDLINE_MAX] = "";
  char *str;
  u_int i = 0;
  u_int j = 0;
  bool hit_output_redir = false;

  for (i = 0; i < strlen(cmdStr) + 1; i++)
  {
    if (cmdStr[i] != ' ' &&
        cmdStr[i] != '\0' &&
        cmdStr[i] != '>')
    {

      /* Normal char */
      char temp[2] = {cmdStr[i], '\0'};
      str = strcat(cmd, temp);
      // printf("%s\n", str);
    }
    else
    {
      // printf("got into else\n");
      /* Hit space, meta-char, or endl */
      if (cmdStr[i] == ' ')
      {
        // printf("got into space\n");
        if (!hit_output_redir)
        {
          // printf("got into add to args\n");
          command->args[j] = malloc(sizeof(str));
          strcpy(command->args[j], str);
          // command->args[j] = str;
          // printf("added\n");
          // printf("%s\n", command->args[j]);
        }
        else
        {
          // printf("read in space after meta\n");
          continue;
        }
      }
      else if (cmdStr[i] == '>')
      {
        // printf("read in output redir\n");
        command->needs_output_redir = true;
        hit_output_redir = true;
      }
      else if (cmdStr[i] == '\0')
      {
        // printf("got into endl\n");
        if (hit_output_redir)
        {
          command->filename = malloc(sizeof(char *));
          strcpy(command->filename, str);
          // command->filename = str;
        }
        else
        {
          // printf("no output redir\n");
          command->args[j] = malloc(sizeof(str));
          strcpy(command->args[j], str);
        }
      }

      if (cmd[0] != '\0')
      {
        // printf("clearing cmd\n");
        cmd[0] = '\0';
        // printf("%s\n", command->args[j]);
        j++;
        // printf("cmd cleared and j inced\n");
      }
    }
  }

  command->prefix = malloc(sizeof(char *));
  strcpy(command->prefix, command->args[0]);
  // printf("%s\n", command->prefix);
  // if (command->needs_output_redir) {
  //   printf("%s\n", command->filename);
  // }
  // if (command->args[1]) {
  //   printf("%s\n", command->args[1]);
  // }
  // exit(0);

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
    char *cmdStrings[CMDS_MAX];
    // struct Command *commands[CMDS_MAX];
    int retval;
    int status;
    int fd;
    pid_t pid;

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

    int cur_job = 0;
    char *tok = strtok(fxn, "|");

    while (tok)
    {
      tok = trim(tok);
      cmdStrings[cur_job] = malloc(sizeof(tok));
      strcpy(cmdStrings[cur_job], tok);
      printf("%s\n", cmdStrings[cur_job]);

      tok = strtok(NULL, "|");
      cur_job++;
    }
    exit(0);
    command = parseCommand(fxn);

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
        if (command->needs_output_redir)
        {
          strcat(command->filename, ".txt");
          fd = open(command->filename, O_CREAT | O_TRUNC | O_RDWR, 0644);
          dup2(fd, STDOUT_FILENO);
          close(fd);
        }
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