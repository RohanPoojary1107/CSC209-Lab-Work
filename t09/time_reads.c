#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

// Message to print in the signal handling function.
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed.
 * These have to be global so that signal handler to be written will have
 * access.
 */
long num_reads = 0, seconds;

void handler(int code)
{
  fprintf(stderr, MESSAGE, num_reads, seconds);
  exit(0);
}

int main(int argc, char **argv)
{
  struct sigaction newact;
  struct itimerval it_val;
  memset(&newact, 0, sizeof(it_val));
  newact.sa_handler = handler;
  sigemptyset(&newact.sa_mask);
  sigaction(SIGPROF, &newact, NULL);

  if (argc != 3)
  {
    fprintf(stderr, "Usage: time_reads s filename\n");
    exit(1);
  }
  seconds = strtol(argv[1], NULL, 10);

  FILE *fp;
  if ((fp = fopen(argv[2], "r+")) == NULL)
  { // Read+Write for later ...
    perror("fopen");
    exit(1);
  }

  it_val.it_value.tv_sec = seconds;
  it_val.it_value.tv_usec = 0;
  it_val.it_interval.tv_sec = 0;
  it_val.it_interval.tv_usec = 0;

  if (setitimer(ITIMER_PROF, &it_val, NULL) == -1)
  {
    perror("setittimer");
    exit(1);
  }

  /* In an infinite loop, read an int from a random location in the file
     * and print it to stderr.
     */
  for (;;)
  {
    int random_um = (rand() % (99 - 0 + 1));
    fseek(fp, sizeof(int) * random_um, SEEK_SET);
    int num;
    fread(&num, sizeof(int), 1, fp);
    num_reads++;
    printf("%d\n", num);
  }

  if (fclose(fp) != 0)
  {
    perror("fclose");
    return 1;
  }
  return 1; //something is wrong if we ever get here!
}
