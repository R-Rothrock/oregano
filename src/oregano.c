// turmeric.c
// https://github.com/R-Rothrock/turmeric

// Linux only

#define PATHNAME "./a.out"

#include<stdlib.h>
#include<sys/ptrace.h>
#include<sys/reg.h>
#include<sys/stat.h>
#include<sys/types.h> /* pid_t and target_addr_t */
#include<unistd.h>

#if defined(__x86_64) // amd64
#define OFFSET 1
#define IP_REG RIP

#elif defined(__i836) // amd32
#define OFFSET 1
#define IP_REG EIP

#elif defined(__aarch64__) // arm64
#define OFFSET -1
#define IP_REG RIP

#elif defined(__arm__) // arm32
#define OFFSET -1
#define IP_REG EIP

#endif

unsigned int get_file_size(char *pathname)
{
  struct stat stats;
  stat(pathname, &stats);
  return stats.st_size;
}

void child()
{
  char *argv[] = {"/usr/bin/htop", NULL};
  char *envp[] = {NULL};
  exit(execve(argv[0], argv, envp));
}

void attach(pid_t pid)
{
  int _;
  ptrace(PTRACE_ATTACH, pid);
  waitpid(pid, &_, 0);
}

target_addr_t get_rip_val(pid_t pid)
{
  long ret = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * IP_REG);
  return target_addr_t * (ret - OFFSET);
}

int main(int argc, char *argv[])
{
  pid_t pid = fork();

  if(pid == 0)
  {
    child();
  }
  else if(pid < 0)
  {
    return -1;
  }

  attach(pid);

  get_ip(pid):
}
