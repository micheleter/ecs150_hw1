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
  char *args[ARG_MAX + 2];
  bool needs_output_redir;
  char *filename;
} Command;

// void append(char *s, char c)
// {
//         int len = strlen(s);
//         s[len] = c;
//         s[len + 1] = '\0';
// }

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
<<<<<<< HEAD
        cmdStr[i] != '>')
    {
=======
        cmdStr[i] != '>') {
>>>>>>> ff93865d86441ce1131cdd1b3bd982ebe441db98

      /* Normal char */
      char temp[2] = {cmdStr[i], '\0'};
      str = strcat(cmd, temp);
      // printf("%s\n", str);
    }
<<<<<<< HEAD
    else
    {
      printf("got into else\n");
      /* Hit space, meta-char, or endl */
      if (cmdStr[i] == ' ')
      {
        printf("got into space\n");
        if (!hit_output_redir)
        {
=======
    else {
      // printf("got into else\n");
      /* Hit space, meta-char, or endl */
      if (cmdStr[i] == ' ') {
        // printf("got into space\n");
        if (!hit_output_redir) {
<<<<<<< HEAD
>>>>>>> ff93865d86441ce1131cdd1b3bd982ebe441db98
          printf("got into add to args\n");
=======
          // printf("got into add to args\n");
>>>>>>> bc993fb7225e7f9ad02d2e223dc14f2895180b85
          command->args[j] = malloc(sizeof(str) + 1);
          strcpy(command->args[j], str);
          // printf("added\n");
          // printf("%s\n", command->args[j]);
        }
<<<<<<< HEAD
        else
        {
=======
        else {
<<<<<<< HEAD
>>>>>>> ff93865d86441ce1131cdd1b3bd982ebe441db98
          printf("read in space after meta\n");
=======
          // printf("read in space after meta\n");
>>>>>>> bc993fb7225e7f9ad02d2e223dc14f2895180b85
          continue;
        }
      }
<<<<<<< HEAD
      else if (cmdStr[i] == '>')
      {
=======
      else if (cmdStr[i] == '>') {
<<<<<<< HEAD
>>>>>>> ff93865d86441ce1131cdd1b3bd982ebe441db98
        printf("read in output redir\n");
=======
        // printf("read in output redir\n");
>>>>>>> bc993fb7225e7f9ad02d2e223dc14f2895180b85
        command->needs_output_redir = true;
        hit_output_redir = true;
      }
<<<<<<< HEAD
      else if (cmdStr[i] == '\0')
      {
        if (hit_output_redir)
        {
          command->filename = malloc(sizeof(char *));
          strcpy(command->filename, str);
        }
        else
        {
=======
      else if (cmdStr[i] == '\0') {
        if (hit_output_redir) {
          command->filename = malloc(sizeof(char*));
          strcpy(command->filename, str);
        }
        else {
>>>>>>> ff93865d86441ce1131cdd1b3bd982ebe441db98
          command->args[j] = malloc(sizeof(str) + 1);
          strcpy(command->args[j], str);
        }
      }

<<<<<<< HEAD
      if (cmd[0] != '\0')
      {
=======
      if (cmd[0] != '\0') {
<<<<<<< HEAD
>>>>>>> ff93865d86441ce1131cdd1b3bd982ebe441db98
        printf("clearing cmd\n");
=======
        // printf("clearing cmd\n");
>>>>>>> bc993fb7225e7f9ad02d2e223dc14f2895180b85
        cmd[0] = '\0';
        // printf("%s\n", command->args[j]);
        j++;
        // printf("cmd cleared and j inced\n");
      }
    }
  }
  command->args[j] = malloc(sizeof(char));
  strcpy(command->args[j], "\0");
<<<<<<< HEAD
  command->prefix = malloc(sizeof(char *));
  strcpy(command->prefix, command->args[0]);
  printf("%s\n", command->prefix);
  if (command->needs_output_redir)
  {
=======
  command->prefix = malloc(sizeof(char*));
  strcpy(command->prefix, command->args[0]);
<<<<<<< HEAD
  printf("%s\n", command->prefix);
  if (command->needs_output_redir) {
>>>>>>> ff93865d86441ce1131cdd1b3bd982ebe441db98
    printf("%s\n", command->filename);
  }
  exit(0);
=======
  // printf("%s\n", command->prefix);
  // if (command->needs_output_redir) {
  //   printf("%s\n", command->filename);
  // }
  // exit(0);
>>>>>>> bc993fb7225e7f9ad02d2e223dc14f2895180b85

  return command;

  /* Original method - works */

  // struct Command *command = malloc(sizeof(struct Command));
  //
  // int i = 1;
  // char *cur = strtok(cmdStr, " ");
  // command->args[0] = cur;
  // command->prefix = cur;
  //
  // while (cur)
  // {
  //   cur = strtok(NULL, " ");
  //   command->args[i] = cur;
  //   i++;
  // }
  // command->args[i] = NULL;
  //
  // return command;
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
    int retval;
    int status;
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