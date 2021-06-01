#include "ptrace.h"
#include <stddef.h>

long ptrace_attach(pid_t pid)
{
	return ptrace(PTRACE_ATTACH, pid, NULL, NULL);
}

long ptrace_detach(pid_t pid)
{
	return ptrace(PTRACE_DETACH, pid, NULL, NULL);
}

long ptrace_read(pid_t pid, void *src, void *dst, size_t size)
{
	size_t i;

	for (i = 0; i < size; ++i) {
		long data;

		data = ptrace(PTRACE_PEEKDATA, pid, NULL, &((char *)src)[i]);
		*(&((char *)dst)[i]) = (char)data;
	}

	return (long)(i == size ? 0 : -1);
}

long ptrace_write(pid_t pid, void *dst, void *src, size_t size)
{
	size_t i;

	for (i = 0; i < size; ++i) {
		long data;

		data = ptrace(PTRACE_PEEKDATA, pid, NULL, &((char *)src)[i]);
		*(char *)&data = *(&((char *)dst)[i]);
		if (ptrace(PTRACE_POKEDATA, pid,
			   data, &((char *)src)[i]) == -1) {
			break;
		}
	}

	return (long)(i == size ? 0 : -1);
}

long ptrace_getregs(pid_t pid, struct user_regs_struct *regs)
{
	return ptrace(PTRACE_GETREGS, pid, NULL, regs);
}

long ptrace_setregs(pid_t pid, struct user_regs_struct *regs)
{
	return ptrace(PTRACE_SETREGS, pid, NULL, regs);
}
