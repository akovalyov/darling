#define __SYSCALL_32BIT_ARG_BYTES 28
#include "SYS.h"

#ifndef SYS_proc_info
#error "SYS_proc_info not defined. The header files libsyscall is building against do not match syscalls.master."
#endif

#if defined(__i386__)
__SYSCALL2(__proc_info, proc_info, 6, cerror_nocancel)
#endif

