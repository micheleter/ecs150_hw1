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
  char *args[ARG_MAX];
  int sizeOfArgs;
  bool needs_output_redir;
  char *filename;
} Command;

bool checkFileExists(struct Command **commands, int numCommands)
{
  if (commands[numCommands - 1]->needs_output_redir)
  {
    if (access(commands[numCommands - 1]->filename, F_OK) == -1)
    {
      return false;
    }
  }
  return true;
}

bool checkForFile(struct Command **commands, int numCommands, char *fxn)
{
  if (commands[numCommands - 1]->needs_output_redir)
  {
    char *tok = strtok(fxn, ">");
    tok = strtok(NULL, " ");
    if (!tok)
    {
      return false;
    }
  }
  return true;
}

bool checkOutputRed(struct Command **commands, int numCommands)
{
  for (int i = 0; i < numCommands; i++)
  {
    if ((commands[i]->needs_output_redir) && (i != numCommands - 1))
    {
      return false;
    }
  }
  return true;
}

bool checkCommand(struct Command **commands, int numCommands)
{
  for (int i = 0; i < numCommands; i++)
  {
    if (!commands[i]->prefix)
    {
      return false;
    }
  }
  return true;
}

struct Node
{
  char *dir;
  struct Node *next;
} Node;

void addNode(struct Node **root, char *directory)
{
  struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
  newNode->dir = (char *)malloc(sizeof(directory));
  strcpy(newNode->dir, directory);
  newNode->next = (*root);
  (*root) = newNode;
}

bool checkArgSize(char *fxn)
{
  int total = 0;

  char *tok = strtok(fxn, " |>&");
  while (tok)
  {
    total++;
    tok = strtok(NULL, " |>&");
  }

  return (total <= ARG_MAX);
}

char *trim(char *str)
{
  if (str[0] == ' ')
  {
    memmove(str, str + 1, strlen(str));
  }
  if (str[strlen(str) - 1] == ' ')
  {
    str[strlen(str) - 1] = '\0';
  }
  return str;
}

struct Command *parseCommand(char *cmdStr)
{

  struct Command *command = malloc(sizeof(struct Command));
  if (!command)
  {
    exit(1);
  }
  command->needs_output_redir = false;
  char cmd[CMDLINE_MAX] = "";
  char *str;
  u_int i = 0;
  u_int j = 0;
  bool hit_output_redir = false;

  for (i = 0; i < strlen(cmdStr) + 1; i++)
  {
    if (cmdStr[i] != ' ' && cmdStr[i] != '\0' && cmdStr[i] != '>')
    {
      /* Normal char */
      char temp[2] = {cmdStr[i], '\0'};
      str = strcat(cmd, temp);
    }
    else
    {
      /* Hit space, meta-char, or endl */
      if (cmdStr[i] == ' ')
      {
        if (!hit_output_redir)
        {
          command->args[j] = malloc(sizeof(str));
          if (command->args[j] == NULL)
          {
            exit(1);
          }
          strcpy(command->args[j], str);
          command->sizeOfArgs += 1;
        }
        else
        {
          continue;
        }
      }
      else if (cmdStr[i] == '>')
      {
        if (i == 0)
        {
          fprintf(stderr, "Error: missing command\n");
          return NULL;
        }
        else if (cmdStr[i - 1] != ' ')
        {
          command->args[j] = malloc(sizeof(str));
          if (command->args[j] == NULL)
          {
            exit(1);
          }
          strcpy(command->args[j], str);
          command->sizeOfArgs += 1;
        }
        command->needs_output_redir = true;
        hit_output_redir = true;
      }
      else if (cmdStr[i] == '\0')
      {
        if (hit_output_redir)
        {
          command->filename = malloc(sizeof(char *));
          if (command->filename == NULL)
          {
            exit(1);
          }
          strcpy(command->filename, str);
        }
        else
        {
          command->args[j] = malloc(sizeof(str));
          if (command->args[j] == NULL)
          {
            exit(1);
          }
          strcpy(command->args[j], str);
          command->sizeOfArgs += 1;
        }
      }
      if (cmd[0] != '\0')
      {
        cmd[0] = '\0';
        j++;
      }
    }
  }
  command->prefix = malloc(sizeof(char *));
  if (command->prefix == NULL)
  {
    exit(1);
  }
  strcpy(command->prefix, command->args[0]);
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

void outputRedirection(struct Command **commands, int numCommands, int fd)
{
  if (commands[numCommands - 1]->needs_output_redir)
  {
    if (commands[numCommands - 1]->filename != NULL)
    {
      fd = open(commands[numCommands - 1]->filename, O_CREAT | O_TRUNC | O_RDWR, 0644);
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
  }
}

void executeCommand(struct Command **commands, char *cmd, int numCommands)
{
  int status;
  int fd = 0;
  int pfd1[2];
  int pfd2[2];
  int pfd3[2];
  pid_t pid[4];
  pid_t wpid;

  /* Regular command */
  /* Create pipes if neccessary */
  if (numCommands > 1)
  {
    pipe(pfd1);
    if (numCommands > 2)
    {
      pipe(pfd2);
      if (numCommands > 3)
      {
        pipe(pfd3);
      }
    }
  }

  pid[0] = fork(); // Init and child
  if (pid[0] == -1)
  {
    exit(1);
  }
  if (pid[0] > 0) // Parent
  {
    if (numCommands >= 2)
    {
      pid[1] = fork();
      if (pid[1] == -1)
      {
        exit(1);
      }
      if (pid[1] > 0)
      {
        // Parent
        close(pfd1[1]);
        if (numCommands >= 3)
        {
          pid[2] = fork();
          if (pid[2] == -1)
          {
            exit(1);
          }
          if (pid[2] > 0)
          {
            // Parent
            close(pfd2[1]);
            if (numCommands >= 4)
            {
              pid[3] = fork();
              if (pid[3] == -1)
              {
                exit(1);
              }
              if (pid[3] > 0)
              {
                // Parent
                close(pfd3[1]);
              }
              else if (pid[3] == 0)
              {
                // Child
                close(pfd1[0]);
                close(pfd1[1]);
                close(pfd2[0]);
                close(pfd2[1]);

                if (commands[3]->needs_output_redir)
                {
                  if (numCommands == 4)
                  {
                    outputRedirection(commands, numCommands, fd);
                  }
                }
                close(STDERR_FILENO);
                close(pfd3[1]);
                dup2(pfd3[0], STDIN_FILENO);
                close(pfd3[0]);
                execvp(commands[3]->prefix, commands[3]->args);
                perror("execvp");
                exit(1);
              }
              else
              {
                // Error
                perror("fork");
                exit(1);
              }
            }
          }
          else if (pid[2] == 0)
          {
            // Child
            close(pfd1[0]);
            close(pfd1[1]);

            if (commands[2]->needs_output_redir)
            {
              if (numCommands == 3)
              {
                outputRedirection(commands, numCommands, fd);
              }
            }
            close(STDERR_FILENO);
            close(pfd2[1]);
            dup2(pfd2[0], STDIN_FILENO);
            close(pfd2[0]);
            if (numCommands > 3)
            {
              close(pfd3[0]);
              dup2(pfd3[1], STDOUT_FILENO);
              close(pfd3[1]);
            }
            execvp(commands[2]->prefix, commands[2]->args);
            perror("execvp");
            exit(1);
          }
          else
          {
            // Error
            perror("fork");
            exit(1);
          }
        }
      }
      else if (pid[1] == 0)
      {
        // Child 2
        close(STDERR_FILENO);
        close(pfd1[1]);
        dup2(pfd1[0], STDIN_FILENO);
        close(pfd1[0]);
        if (numCommands > 2)
        {
          close(pfd2[0]);
          dup2(pfd2[1], STDOUT_FILENO);
          close(pfd2[1]);
        }
        execvp(commands[1]->prefix, commands[1]->args);
        perror("execvp");
        exit(1);
      }
      else
      {
        // Error
        perror("fork");
        exit(1);
      }
    }
    while ((wpid = wait(&status)) > 0)
    {
    }
    if (WEXITSTATUS(status) > 0)
    {
      fprintf(stderr, "Error: command not found\n");
    }
    print_completion(cmd, WEXITSTATUS(status));
  }
  else if (pid[0] == 0) // Child 1 executing
  {
    /* Piping */
    if (numCommands >= 2)
    {
      if (commands[1]->needs_output_redir)
      {
        if (numCommands == 2)
        {
          outputRedirection(commands, numCommands, fd);
        }
      }
      close(STDERR_FILENO);
      close(pfd1[0]);
      dup2(pfd1[1], STDOUT_FILENO);
      close(pfd1[1]);
      execvp(commands[0]->prefix, commands[0]->args);
      perror("execvp");
      exit(1);
    }

    if (commands[0]->needs_output_redir)
    {
      if (numCommands == 1)
      {
        outputRedirection(commands, numCommands, fd);
      }
    }

    /* Not piping */
    close(STDERR_FILENO);
    execvp(commands[0]->prefix, commands[0]->args);
    perror("execvp");
    exit(1);
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
  struct Node *head = NULL;

  while (1)
  {
    char *nl;
    char fxn[CMDLINE_MAX];
    char fxn2[CMDLINE_MAX];
    char fxn3[CMDLINE_MAX];
    char *cmdStrings[CMDS_MAX];
    struct Command *commands[CMDS_MAX];
    int retval;
    int cur_job = 0;

    /* Print prompt */
    printf("sshell$ ");
    fflush(stdout);

    /* Get command line */
    fgets(cmd, CMDLINE_MAX, stdin);

    // Check if any command is even entered
    if (cmd[0] == '\n' || cmd[0] == ' ')
    {
      continue;
    }

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
    strcpy(fxn2, cmd);
    strcpy(fxn3, cmd);

    // Pass fxn, pass commands arr, return cur_job
    char *tok = strtok(fxn, "|");
    while (tok)
    {
      tok = trim(tok);
      cmdStrings[cur_job] = malloc(sizeof(tok) * sizeof(char));
      if (cmdStrings[cur_job] == NULL)
      {
        exit(1);
      }
      commands[cur_job] = (struct Command *)malloc(sizeof(struct Command *));
      strcpy(cmdStrings[cur_job], tok);
      if (commands[cur_job] == NULL)
      {
        exit(1);
      }
      commands[cur_job] = parseCommand(cmdStrings[cur_job]);
      tok = strtok(NULL, "|");
      cur_job++;
    }

    if (!checkForFile(commands, cur_job, fxn3))
    {
      fprintf(stderr, "Error: no output file\n");
      continue;
    }

    // if (!checkFileExists(commands, cur_job))
    // {
    //   fprintf(stderr, "Error: cannot open output file\n");
    //   continue;
    // }

    if (!checkCommand(commands, cur_job))
    {
      fprintf(stderr, "Error: missing command\n");
      continue;
    }
    if (!checkArgSize(fxn2))
    {
      fprintf(stderr, "Error: too many process arguments\n");
      continue;
    }
    if (!checkOutputRed(commands, cur_job))
    {
      fprintf(stderr, "Error: mislocated output redirection\n");
      continue;
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
    }
    else if (!strcmp(commands[0]->prefix, "cd"))
    {
      retval = cdBuiltIn(commands[0]->args[1]);
      print_completion(cmd, retval);
    }
    else if (!strcmp(commands[0]->prefix, "pushd"))
    {
      char buf[CMDLINE_MAX];
      int retval = cdBuiltIn(commands[0]->args[1]);
      char *ndir = getcwd(buf, (size_t)CMDLINE_MAX);

      addNode(&head, ndir);
      print_completion(cmd, retval);
    }
    else if (!strcmp(cmd, "popd"))
    {
      int retval = cdBuiltIn(head->dir);
      struct Node *tmp;
      if (head != NULL)
      {
        tmp = head;
        head = head->next;
        free(tmp);
      }
      print_completion(cmd, retval);
    }
    else if (!strcmp(cmd, "dirs"))
    {
      struct Node *node = head;
      char buf[CMDLINE_MAX];
      char *cur = getcwd(buf, (size_t)CMDLINE_MAX);
      printf("%s\n", cur);

      while (node != NULL)
      {
        printf("%s\n", node->dir);
        node = node->next;
      }
    }
    else
    {
      executeCommand(commands, cmd, cur_job);
    }
  }
  return EXIT_SUCCESS;
}
