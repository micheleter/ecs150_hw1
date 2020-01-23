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
        if (cmdStr[i - 1] != ' ')
        {
          command->args[j] = malloc(sizeof(str));
          strcpy(command->args[j], str);
        }
        // printf("read in output redir\n");
        command->needs_output_redir = true;
        hit_output_redir = true;
      }
      else if (cmdStr[i] == '\0')
      {
        // printf("got into endl\n");
        if (hit_output_redir)
        {
          // printf("saving filename\n");
          command->filename = malloc(sizeof(char *));
          strcpy(command->filename, str);
          // printf("filename saved\n");
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
  // if (command->needs_output_redir)
  // {
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

int exitBuiltIn()
{
  fprintf(stderr, "Bye...\n");
  return 0;
}

int cdBuiltIn(char *dir)
{
  int retval = 0;
  if (dir)
  {
    int exitVal = chdir(dir);
    if (exitVal != 0)
    {
      retval = 1;
      fprintf(stderr, "Error: no such directory\n");
    }
  }
  return retval;
}

int pwdBuiltIn()
{
  int retval = 0;
  char buf[CMDLINE_MAX];
  char *dir = getcwd(buf, (size_t)CMDLINE_MAX);
  if (dir)
  {
    printf("%s\n", dir);
  }
  else
  {
    retval = 1;
    perror("getcwd");
  }
  return retval;
}

// void outputRedirection() {
//   if (commands[numCommands-1]->needs_output_redir)
//   {
//     if (commands[numCommands-1]->filename != NULL)
//     {
//       strcat(commands[numCommands-1]->filename, ".txt");
//       fd = open(commands[numCommands-1]->filename, O_CREAT | O_TRUNC | O_RDWR, 0644);
//       dup2(fd, STDOUT_FILENO);
//       close(fd);
//     }
//     else
//     {
//       fprintf(stderr, "Error: no output file\n");
//     }
//   }
// }

void executeCommand(struct Command **commands, char *cmd, int numCommands)
{
  int status;
  // int fd;
  int pfd[2];
  pid_t pid[4];

  /* Regular command */
  /* Create pipe if neccessary */
  if (numCommands > 1)
  {
    pipe(pfd);
  }

  pid[0] = fork(); // Init and child
  if (pid[0] == 0)
  {
    // Child 1 executing

    /* Piping */
    if (numCommands >= 2)
    {
      char buf[CMDLINE_MAX];

      printf("%d\n", pfd[0]);
      printf("%d\n", pfd[1]);
      // close(pfd[0]);
      printf("closed read\n");
      dup2(pfd[1], STDOUT_FILENO);
      close(pfd[1]);
      fprintf(stderr, "dup2ed\n");
      printf("hello");
      fflush(stdout);
      fprintf(stderr, "about to fork again\n");

      // pipe(pfd);
      pid[1] = fork();

      if (pid[1] == 0)
      {
        // Child 2 executing
        fprintf(stderr, "%d\n", pfd[0]);
        fprintf(stderr, "%d\n", pfd[1]);
        // close(pfd[1]);
        fprintf(stderr, "closed write\n");
        dup2(pfd[0], STDIN_FILENO);
        fprintf(stderr, "dup2ed again\n");
        close(pfd[0]);
        fprintf(stderr, "closed read2\n");
        read(STDIN_FILENO, buf, CMDLINE_MAX);
        fprintf(stderr, "%s\n", buf);

        // execvp(commands[1]->prefix, commands[1]->args);
        // perror("execvp");
        exit(1);
      }
      else if (pid[1] > 0)
      {
        // Parent executing
        // waitpid(-1, &status, 0);
        wait(NULL);
        // wait(NULL);
        // print_completion(cmd, status);
        printf("does this work?\n");
      }
      else
      {
        // Error
        perror("fork");
        exit(1);
      }

      // execvp(commands[0]->prefix, commands[0]->args);
      // perror("execvp");
      // exit(1);
    }

    /* Not piping */
    // printf("not piping\n");
    // printf("%s\n", commands[0]->prefix);
    // exit(0);
    execvp(commands[0]->prefix, commands[0]->args);
    perror("execvp");
    exit(1);
  }
  else if (pid[0] > 0)
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

int main(void)
{

  char cmd[CMDLINE_MAX];

  while (1)
  {
    char *nl;
    char fxn[CMDLINE_MAX];
    char *cmdStrings[CMDS_MAX];
    struct Command *commands[CMDS_MAX];
    int retval;
    int cur_job = 0;
    // bool regularCommand = true;

    /* Print prompt */
    printf("sshell$ ");
    fflush(stdout);

    /* Get command line */
    fgets(cmd, CMDLINE_MAX, stdin);
    // struct Command *command;

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

    char *tok = strtok(fxn, "|");

    while (tok)
    {
      tok = trim(tok);
      cmdStrings[cur_job] = malloc(sizeof(tok));
      strcpy(cmdStrings[cur_job], tok);
      commands[cur_job] = malloc(sizeof(struct Command *));
      commands[cur_job] = parseCommand(cmdStrings[cur_job]);
      tok = strtok(NULL, "|");
      cur_job++;
    }

    /* Builtin commands */
    if (!strcmp(cmd, "exit"))
    {
      retval = exitBuiltIn();
      print_completion(cmd, retval);
      break;
    }
    else if (!strcmp(cmd, "pwd"))
    {
      retval = pwdBuiltIn();
      print_completion(cmd, retval);
      // regularCommand = false;
    }
    else if (!strcmp(commands[0]->prefix, "cd"))
    {
      retval = cdBuiltIn(commands[0]->args[1]);
      print_completion(cmd, retval);
      // regularCommand = false;
    }

    else
    {
      // printf("gets in to regular command\n");
      // printf("%d\n", cur_job);
      // exit(0);
      executeCommand(commands, cmd, cur_job);
    }
  }
  return EXIT_SUCCESS;
}