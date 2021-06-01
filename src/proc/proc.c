#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "proc.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>

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
	int     ret = 0;
	char   *buf;
	ssize_t len;
	char    exe_path[64] = { 0 };

	buf = calloc(PATH_MAX + 1, sizeof(char));
	if (!buf)
		return ret;

	snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", proc->id);

	len = readlink(exe_path, buf, PATH_MAX);

	if (len != -1) {
		proc->path = calloc(len + 1, sizeof(char));
		if (proc->path) {
			strncpy(proc->path, buf, len);
			proc->path[len] = '\x00';
			ret = !ret;
		}
	}

	free(buf);
	return ret;
}

static int read_cwd(struct proc_t *proc)
{
	int     ret = 0;
	char   *buf;
	ssize_t len;
	char    cwd_path[64] = { 0 };

	buf = calloc(PATH_MAX + 1, sizeof(char));
	if (!buf)
		return ret;

	snprintf(cwd_path, sizeof(cwd_path), "/proc/%d/cwd", proc->id);

	len = readlink(cwd_path, buf, PATH_MAX);

	if (len != -1) {
		proc->cwd = calloc(len + 1, sizeof(char));
		if (proc->cwd) {
			strncpy(proc->cwd, buf, len);
			proc->cwd[len] = '\x00';
			ret = !ret;
		}
	}

	free(buf);

	return ret;
}

static int read_comm(struct proc_t *proc)
{
	int  ret = 0;
	int  fd;
	char buf[64] = { 0 };
	ssize_t size;
	ssize_t total = 0;

	snprintf(buf, sizeof(buf), "/proc/%d/comm", proc->id);

	fd = open(buf, O_RDONLY);
	if (fd == -1)
		return ret;

	while ((size = read(fd, (void *)buf, sizeof(buf)))) {
		char *old_name = proc->name;
		size /= sizeof(char);
		proc->name = calloc(total + size, sizeof(char));

		if (total) {
			if (proc->name)
				strncpy(proc->name, old_name, total);
			free(old_name);
		}

		strncpy(&proc->name[total], buf, size);
		total += size;
	}
	
	if (total) {
		proc->name[total - 1] = '\x00'; /* Replace '\n' with '\x00' */
		ret = !ret;
	}

	close(fd);
	return ret;
}

static int read_cmdline(struct proc_t *proc)
{
	int  ret = 0;
	int  fd;
	char buf[64] = { 0 };
	ssize_t size;
	ssize_t total = 0;

	snprintf(buf, sizeof(buf), "/proc/%d/cmdline", proc->id);

	fd = open(buf, O_RDONLY);
	if (fd == -1)
		return ret;

	while ((size = read(fd, (void *)buf, sizeof(buf)))) {
		char *old_cmdline = proc->cmdline;
		size /= sizeof(char);
		proc->cmdline = calloc(total + size + 1, sizeof(char));

		if (total) {
			if (proc->cmdline)
				strncpy(proc->cmdline, old_cmdline, total);
			free(old_cmdline);
		}

		strncpy(&proc->cmdline[total], buf, size);

		total += size;
	}
	
	if (total) {
		proc->cmdline[total] = '\x00';
		ret = !ret;
	}

	close(fd);
	return ret;
}

static int read_status(struct proc_t *proc)
{
	int ret = 0;
	char *status = (char *)NULL;
	char *ptr;

	{
		int  ret = 0;
		int  fd;
		char buf[64] = { 0 };
		ssize_t size;
		ssize_t total = 0;

		snprintf(buf, sizeof(buf), "/proc/%d/status", proc->id);

		fd = open(buf, O_RDONLY);
		if (fd == -1)
			return ret;

		while ((size = read(fd, (void *)buf, sizeof(buf)))) {
			char *old_status = status;
			size /= sizeof(char);
			status = calloc(total + size + 1, sizeof(char));

			if (total) {
				if (status)
					strncpy(status, old_status, total);
				free(old_status);
			}

			strncpy(&status[total], buf, size);

			total += size;
		}
		
		if (total)
			status[total] = '\x00';

		close(fd);
	}

	if (!status)
		return ret;

	ptr = strstr(status, "State:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->state = *ptr;

	ptr = strstr(ptr, "PPid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->parent = (pid_t)atoi(ptr);

	ptr = strstr(ptr, "TracerPid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->tracer = (pid_t)atoi(ptr);

	ptr = strstr(ptr, "Uid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->owner = (uid_t)atoi(ptr);

	ptr = strstr(ptr, "Threads:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	proc->threads = (int)atoi(ptr);

	ret = !ret;

	free(status);

	return ret;
}
