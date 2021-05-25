#ifndef PROCESS_H
#define PROCESS_H

#include "main.h"

typedef struct process_t {
	pid_t  id;
	pid_t  parent;
	pid_t  tracer;
	char   state;
	char  *exe_path;
	char  *cmdline;
	struct passwd owner;
} process_t;

int  open_process(pid_t pid, process_t *process);
void close_process(process_t *process);

#endif
