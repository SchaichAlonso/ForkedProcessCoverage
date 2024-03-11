#include <sys/wait.h>

#include <unistd.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int recurse(const char *argsv0, int n)
{
  int pipefd[2];
  int retval = pipe(pipefd);

  pid_t pid = fork();
  assert(pid != -1);

  if (pid == 0) {
    close(pipefd[1]);
    dup2(pipefd[0], 1);

    char buffer[64];
    sprintf(buffer, "%d", n);
    execl(argsv0, argsv0, buffer, NULL);
  } 
  
  close(pipefd[0]);

  int status;
  retval = waitpid(pid, &status, 0);
  assert(retval != -1);
  assert(status == EXIT_SUCCESS);

  char buffer[64];
  retval = read(pipefd[1], buffer, sizeof(buffer));
  assert(retval != -1);

  int result;
  retval = sscanf(buffer, "%d", &result);
  assert(retval == 1);
  return result;
}

int main(int argsc, char **argsv)
{
  assert(argsc == 2);

  int n;
  int r = sscanf(argsv[1], "%d", &n);
  assert(r == 1);

  int result = 1;
  if (n > 1) {
    /* fibonacci */
    assert (n <= 10);
    result = recurse(argsv[0], n-2) + recurse(argsv[0], n-1);
  }
  printf("%d\n", result);

  return EXIT_SUCCESS;
}
