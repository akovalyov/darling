#include "sigaction.h"
#include "../base.h"
#include "../errno.h"
#include "../simple.h"
#include <asm/unistd.h>
#include <stddef.h>
#include "../../../../../platform-include/sys/errno.h"

static void handler_linux_to_bsd(int linux_signum, struct linux_siginfo* info, void* ctxt);
static int sigflags_bsd_to_linux(int flags);
static int sigflags_linux_to_bsd(int flags);
extern void sig_restorer(void);

extern void* memcpy(void* dest, const void* src, __SIZE_TYPE__ len);

// Libc uses only one trampoline
void (*sa_tramp)(void*, int, int, struct bsd_siginfo*, void*) = 0;
static bsd_sig_handler* sig_handlers[32];

long sys_sigaction(int signum, const struct bsd___sigaction* nsa, struct bsd_sigaction* osa)
{
	int ret, linux_signum;
	struct linux_sigaction sa, olsa;

	linux_signum = signum_bsd_to_linux(signum);
	if (linux_signum == 0)
		return -EINVAL;

	if (nsa != NULL)
	{
		sa_tramp = nsa->sa_tramp;
		if (nsa->sa_sigaction != SIG_DFL && nsa->sa_sigaction != SIG_IGN
				&& nsa->sa_sigaction != SIG_ERR)
		{
			sa.sa_sigaction = &handler_linux_to_bsd;
		}
		else
			sa.sa_sigaction = (linux_sig_handler*) nsa->sa_sigaction;

		sigset_bsd_to_linux(&nsa->sa_mask, &sa.sa_mask);
		sa.sa_flags = sigflags_bsd_to_linux(nsa->sa_flags) | LINUX_SA_RESTORER;
		sa.sa_restorer = sig_restorer;
	}

	ret = LINUX_SYSCALL(__NR_rt_sigaction, linux_signum,
			(nsa != NULL) ? &sa : NULL, &olsa,
			sizeof(sa.sa_mask));
	if (ret < 0)
		return errno_linux_to_bsd(ret);

	if (osa != NULL)
	{
		if (olsa.sa_sigaction == handler_linux_to_bsd)
			osa->sa_sigaction = sig_handlers[linux_signum];
		else // values such as SIG_DFL
			osa->sa_sigaction = (bsd_sig_handler*) olsa.sa_sigaction;
		sigset_linux_to_bsd(&olsa.sa_mask, &osa->sa_mask);
		osa->sa_flags = sigflags_linux_to_bsd(olsa.sa_flags);
	}
	
	if (nsa != NULL)
	{
		//  __simple_printf("Saving handler for signal %d: %d\n", linux_signum, nsa->sa_sigaction);
		sig_handlers[linux_signum] = nsa->sa_sigaction;
	}

	return 0;
}

static void handler_linux_to_bsd(int linux_signum, struct linux_siginfo* info, void* ctxt)
{
	int bsd_signum;
	struct bsd_siginfo binfo;

	bsd_signum = signum_linux_to_bsd(linux_signum);

	memcpy(&binfo, info, sizeof(binfo));
	binfo.si_signo = signum_linux_to_bsd(binfo.si_signo);
	
	// __simple_printf("Handling signal %d\n", linux_signum);

	sig_handlers[linux_signum](bsd_signum, &binfo, ctxt);
	
	// __simple_printf("Signal handled\n");
}

static int sigflags_bsd_to_linux(int flags)
{
	int linux_flags = LINUX_SA_SIGINFO;

	if (flags & BSD_SA_NOCLDSTOP)
		linux_flags |= LINUX_SA_NOCLDSTOP;
	if (flags & BSD_SA_NOCLDWAIT)
		linux_flags |= LINUX_SA_NOCLDWAIT;
	if (flags & BSD_SA_ONSTACK)
		linux_flags |= LINUX_SA_ONSTACK;
	if (flags & BSD_SA_NODEFER)
		linux_flags |= LINUX_SA_NODEFER;
	if (flags & BSD_SA_RESETHAND)
		linux_flags |= LINUX_SA_RESETHAND;
	if (flags & BSD_SA_RESTART)
		linux_flags |= LINUX_SA_RESTART;

	return linux_flags;
}

static int sigflags_linux_to_bsd(int flags)
{
	int bsd_flags = 0;

	if (flags & LINUX_SA_NOCLDSTOP)
		bsd_flags |= BSD_SA_NOCLDSTOP;
	if (flags & LINUX_SA_NOCLDWAIT)
		bsd_flags |= BSD_SA_NOCLDWAIT;
	if (flags & LINUX_SA_ONSTACK)
		bsd_flags |= BSD_SA_ONSTACK;
	if (flags & LINUX_SA_NODEFER)
		bsd_flags |= BSD_SA_NODEFER;
	if (flags & LINUX_SA_RESETHAND)
		bsd_flags |= BSD_SA_RESETHAND;
	if (flags & LINUX_SA_RESTART)
		bsd_flags |= BSD_SA_RESTART;

	return bsd_flags;
}

