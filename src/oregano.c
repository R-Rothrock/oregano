// oregano.c
// https://github.com/R-Rothrock/oregano

// Linux only

#include<stdlib.h>
#include<sys/ptrace.h> /* ptrace() */
#include<sys/reg.h> /* EIP, RIP */
#include<sys/stat.h> /* struct stat, stat() */
#include<sys/types.h> /* pid_t */
#include<sys/wait.h> /* waitpid() */
#include<unistd.h>

#if defined(__x86_64) // amd64
#define OFFSET -1
#define IP_REG RIP
#define PTR_T u_int64_t

#elif defined(__i836) // amd32
#define OFFSET -1
#define IP_REG EIP
#define PTR_T uint32_t

#elif defined(__aarch64__) // arm64
#define OFFSET 1
#define IP_REG RIP
#define PTR_T u_int64_t

#elif defined(__arm__) // arm32
#define OFFSET 1
#define IP_REG EIP
#define PTR_T uint32_t

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

PTR_T get_ip_reg(pid_t pid)
{
  long ret = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * IP_REG);
  return (PTR_T) (ret + OFFSET);
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

  PTR_T ip = get_ip_reg(pid);
}
