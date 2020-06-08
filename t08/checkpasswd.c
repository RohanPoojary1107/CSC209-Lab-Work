#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void)
{
  char user_id[MAXLINE];
  char password[MAXLINE];

  if (fgets(user_id, MAXLINE, stdin) == NULL)
  {
    perror("fgets");
    exit(1);
  }
  if (fgets(password, MAXLINE, stdin) == NULL)
  {
    perror("fgets");
    exit(1);
  }

  if (strlen(user_id) > MAX_PASSWORD + 1)
  {
    printf("%s", NO_USER);
    exit(1);
  }

  if (strlen(password) > MAX_PASSWORD + 1)
  {
    printf("%s", INVALID);
    exit(1);
  }

  int fd[2], r;

  if ((pipe(fd)) == -1)
  {
    perror("pipe");
    exit(1);
  }
  if ((r = fork()) > 0)
  {
    if ((close(fd[0])) == -1)
    {
      perror("close");
    }
    write(fd[1], user_id, MAX_PASSWORD);
    write(fd[1], password, MAX_PASSWORD);

    if ((close(fd[1])) == -1)
    {
      perror("close");
    }
    int status;
    if (wait(&status))
    {
      int exit_status = WEXITSTATUS(status);
      if (exit_status == 0)
      {
        printf("%s", SUCCESS);
      }
      else if (exit_status == 2)
      {
        printf("%s", INVALID);
      }
      else if (exit_status == 3)
      {
        printf("%s", NO_USER);
      }
      else
      {
        printf("ERROR: Something isn't right!");
      }
    }
  }
  else if (r == 0)
  {
    if ((close(fd[1])) == -1)
    {
      perror("close");
    }
    if ((dup2(fd[0], fileno(stdin))) == -1)
    {
      perror("dup2");
      exit(1);
    }
    if ((close(fd[0])) == -1)
    {
      perror("close");
    }
    execl("./validate", "validate", (char *)0);
    //printf("ERROR: exec should not return\n");
    perror("excel");
    exit(1);
  }
  else
  {
    perror("fork");
    exit(1);
  }
  return 0;
}
