// oregano.c
// https://github.com/R-Rothrock/oregano

#include<stdio.h> // to be commented out on commit

#define _GNU_SOURCE

static char pathname[] = "./test.out";

#include<stdlib.h>
#include<sys/ptrace.h> /* ptrace() */
#include<sys/reg.h> /* EIP, RIP */
#include<sys/types.h> /* pid_t */
#include<sys/wait.h> /* waitpid() */
#include<unistd.h>

#if defined(__x86_64) // amd64
  #define OFFSET -1
  #define IP_REG RIP
  #define PTR_T u_int64_t

  static u_int8_t shellcode1[] = {0xb8, 0x3b, 0x00, 0x00, 0x00, 0x48, 0xbf};
  static u_int8_t shellcode2[] = {
    0x48, 0x31, 0xf6, 0x48, 0x31,
    0xd2, 0x0f, 0x05, 0xcc
  };

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

void child()
{
  //printf("Child is executing...");
  char *child_argv[] = {"/bin/htop", 0};
  char *child_envp[] = {0};
  int ret = execve(child_argv[0], child_argv, child_envp);
  //printf("Child finished execution");
  exit(ret);
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

  //perror("Status"); // to be commented out on commit

  PTR_T ip = get_ip_reg(pid);

  // storing `pathname`

  ip += sizeof(pathname)/sizeof(char);

  PTR_T pathname_addr = ip;
  for(int i = sizeof(pathname)/sizeof(char); i >= 0; i--)
  {
    ptrace(PTRACE_POKETEXT, pid, ip, pathname[i]);
    ip--;
  }

  for(int i = sizeof(shellcode1); i >= 0; i--)
  {
    ptrace(PTRACE_POKETEXT, pid, ip, shellcode1[i]);
    ip--;
  }

  ptrace(PTRACE_POKETEXT, pid, ip, pathname_addr);

  for(int i = sizeof(shellcode2); i >= 0; i--)
  {
    ptrace(PTRACE_POKETEXT, pid, ip, shellcode2[i]);
    ip--;
  }

  ptrace(PTRACE_CONT, pid, 0, 0);

  perror("Status"); // to be commented out on commit

  return 0;
}
