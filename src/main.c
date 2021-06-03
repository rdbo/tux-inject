#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <malloc.h>
#include <limits.h>
#include "proc/proc.h"
#include "maps/maps.h"

int main()
{
	struct proc_t proc;
	struct module_t mod;
	char   *mod_path;

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

	find_module(getpid(), "libc", &mod);
	mod_path = calloc(PATH_MAX, sizeof(char));
	if (!mod_path) {
		printf("[!] Unable to allocate memory\n");
		return -1;
	}

	get_module_path(getpid(), &mod, mod_path, PATH_MAX);
	
	printf("Module Base: %p\n", mod.base);
	printf("Module End:  %p\n", mod.end);
	printf("Module Path  %s\n", mod_path);

	close_proc(&proc);

	getchar();

	return 0;
}
