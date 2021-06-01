#ifndef PTRACE_H
#define PTRACE_H

#include <sys/types.h>
#include <sys/user.h>
#include <sys/ptrace.h>

long ptrace_attach(pid_t pid);
long ptrace_detach(pid_t pid);
long ptrace_read(pid_t pid, void *src, void *dst, size_t size);
long ptrace_write(pid_t pid, void *dst, void *src, size_t size);
long ptrace_getregs(pid_t pid, struct user_regs_struct *regs);
long ptrace_setregs(pid_t pid, struct user_regs_struct *regs);

#endif
