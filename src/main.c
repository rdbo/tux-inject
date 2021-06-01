#include <stdio.h>
#include <unistd.h>
#include "proc/proc.h"

int main()
{
	struct proc_t proc;
	if (open_proc(getpid(), &proc)) {
		printf("[!] Unable to open proc\n");
		return -1;
	}

	printf("PID:\t\t%d\n", proc.id);
	printf("Parent:\t\t%d\n", proc.parent);
	printf("Tracer:\t\t%d\n", proc.tracer);
	printf("Owner:\t\t%d\n", proc.owner);
	printf("Threads:\t%d\n", proc.threads);
	printf("State:\t\t%c\n", proc.state);
	printf("Name:\t\t%s\n", proc.name);
	printf("Path:\t\t%s\n", proc.path);
	printf("CWD:\t\t%s\n", proc.cwd);
	printf("CmdLine:\t%s\n", proc.cmdline);

	close_proc(&proc);
	return 0;
}
