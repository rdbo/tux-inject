#ifndef PROC_H
#define PROC_H

#include <pwd.h>
#include <sys/types.h>

struct proc_t {
	pid_t  id;           /* Process ID */
	pid_t  parent;       /* Parent Process ID */
	pid_t  tracer;       /* Tracer Process ID */
	uid_t  owner;        /* Process Owner */
	int    threads;      /* Thread Count */
	char   state;        /* Process State */
	char  *name;         /* Executable Name */
	char  *path;         /* Executable Path */
	char  *cwd;          /* Current Working Directory */
	char  *cmdline;      /* Command Line */
};

enum {
	PROC_FAILURE = -1, /* Generic error */
	PROC_SUCCESS = 0,  /* Success */
	PROC_EXE_ERR,      /* Failed to read '/proc/<pid>/exe' */
	PROC_CWD_ERR,      /* Failed to read '/proc/<pid>/cwd' */
	PROC_COMM_ERR,     /* Failed to parse '/proc/<pid>/comm' */
	PROC_CMDLINE_ERR,  /* Failed to parse '/proc/<pid>/cmdline' */
	PROC_STATUS_ERR,   /* Failed to parse '/proc/<pid>/status' */
	PROC_DIR_ERR,      /* Unable to open directory '/proc' */
};

int open_proc(pid_t pid, struct proc_t *proc);
void close_proc(struct proc_t *proc);
int enum_procs(int(*callback)(struct proc_t *proc, void *arg),
	       void *arg,
	       int close);

#endif
