#define __SYSCALL_32BIT_ARG_BYTES 4
#include "SYS.h"

#ifndef SYS_pid_resume
#error "SYS_pid_resume not defined. The header files libsyscall is building against do not match syscalls.master."
#endif

#if defined(__i386__)
__SYSCALL2(pid_resume, pid_resume, 1, cerror_nocancel)
#endif

