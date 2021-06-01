#include "proc.h"
#include <dirent.h>

#define SAFE_FREE(ptr) if (ptr) free(ptr)

static int read_exe(struct proc_t *proc);
static int read_cwd(struct proc_t *proc);
static int read_comm(struct proc_t *proc);
static int read_cmdline(struct proc_t *proc);
static int read_status(struct proc_t *proc);

int open_proc(pid_t pid, struct proc_t *proc)
{
	int ret = PROC_FAILURE;

	if (!proc)
		return ret;

	proc->id = pid;

	if (!read_exe(proc)) {
		ret = PROC_EXE_ERR;
		goto L_EXE_ERR;
	}
	if (!read_cwd(proc)) {
		ret = PROC_CWD_ERR;
		goto L_CWD_ERR;
	}
	if (!read_comm(proc)) {
		ret = PROC_COMM_ERR;
		goto L_COMM_ERR;
	}
	if (!read_cmdline(proc)) {
		ret = PROC_CMDLINE_ERR;
		goto L_CMDLINE_ERR;
	}
	if (!read_status(proc)) {
		ret = PROC_STATUS_ERR;
		goto L_STATUS_ERR;
	}	

	goto L_SUCCESS;

	{
	L_STATUS_ERR:
		free(proc->cmdline);
	L_CMDLINE_ERR:
		free(proc->name);
	L_COMM_ERR:
		free(proc->cwd);
	L_CWD_ERR:
		free(proc->path);
	L_EXE_ERR:
		return ret;
	}

L_SUCCESS:
	ret = PROC_SUCCESS;
	return ret;
}

void close_proc(struct proc_t *proc)
{
	if (!proc)
		return;

	SAFE_FREE(proc->cmdline);
	SAFE_FREE(proc->name);
	SAFE_FREE(proc->cwd);
	SAFE_FREE(proc->path);
}

int enum_procs(int(*callback)(struct proc_t *proc, void *arg),
	       void *arg,
	       int close)
{
	DIR *pdir;
	struct dirent *pdirent;

	pdir = opendir("/proc");

	if (!pdir)
		return PROC_DIR_ERR;

	while ((pdirent = readdir(pdir))) {
		pid_t pid = (pid_t)atoi(pdirent->d_name);
		if (pid || (!pid && !strcmp(pdirent->d_name, "0"))) {
			struct proc_t proc;
			if (open_proc(pid, &proc)) {
				int check;

				check = callback(&proc, arg);
				if (close)
					close_proc(&proc);

				if (!check)
					break;
			}
		}
	}

	return PROC_SUCCESS;
}

static int read_exe(struct proc_t *proc)
{

}

static int read_cwd(struct proc_t *proc)
{

}

static int read_comm(struct proc_t *proc)
{

}

static int read_cmdline(struct proc_t *proc)
{

}

static int read_status(struct proc_t *proc)
{

}
