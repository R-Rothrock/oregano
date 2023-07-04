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

#define __DEBUG__

#ifdef __DEBUG__
#  include<stdio.h> // to be commented out on commit
#endif

#define _GNU_SOURCE

static char *pathname[] = "./test.out";

#include<stdlib.h>
#include<sys/ptrace.h> /* ptrace() */
#include<sys/reg.h> /* EIP, RIP */
#include<sys/types.h> /* pid_t */
#include<sys/wait.h> /* waitpid() */
#include<unistd.h>

#if defined(__x86_64) // amd64
#  define OFFSET -1
#  define IP_REG RIP
#  define PTR_T u_int64_t

static u_int8_t shellcode1[] = {
  0xb8, 0x3b, 0x00, 0x00, 0x00, // mov eax, 0x3b ; execve()
  0x48, 0xbf                    // movabs rdi,
};
// *pathname[]
static u_int8_t shellcode2[] = {
  0x48, 0x31, 0xfb,             // xor rsi, rsi
  0x48, 0x31, 0xd2,             // xor rdx, rdx
  0x0f, 0x05,                   // syscall
  0xcc                          // int3 ; ultimately sends SIGKILL
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
  // I'm just going to use the part of the .text section before the
  // instruction pointer.

  PTR_T pathname_addr = ip;

  for(int i = sizeof(*pathname); i >= 0; i--)
  {
    ptrace(PTRACE_POKETEXT, pid, pathname_addr + i, *pathname[sizeof(*pathname)-i]);
  }

  pathname_addr -= sizeof(*pathname);

  for(int i = sizeof(shellcode1); i >= 0; i--)
  {
    ptrace(PTRACE_POKETEXT, pid, ip, shellcode[sizeof(shellcode)-i]);
    ip--;
  }

  ptrace(PTRACE_CONT, pid, 0, 0);

  #ifdef __DEBUG__
  perror("Status");
  #endif

  return 0;
}
