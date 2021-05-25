#include "process.h"

#define CHECK_FREE(ptr) if (ptr) free(ptr)

static ssize_t read_exe_path(process_t *process)
{
	char path[64] = { 0 };
	char *exe_path;
	ssize_t chr_count = 0;

	if (!process)
		return chr_count;

	exe_path = (char *)calloc(PATH_MAX, sizeof(char));

	if (!exe_path)
		return chr_count;

	snprintf(path, ARRLEN(path) - 1, "/proc/%d/exe", process->id);
	chr_count = readlink(path, exe_path, PATH_MAX);

	if (chr_count != -1) {
		process->exe_path = (char *)calloc(chr_count + 1,
							sizeof(char));

		if (process->exe_path) {
			strncpy(process->exe_path, exe_path, chr_count);
			process->exe_path[chr_count] = '\x00';
		} else {
			chr_count = 0;
		}
	}

	free(exe_path);

	if (chr_count == -1)
		chr_count = 0;

	return chr_count;
}

static ssize_t read_cmdline(process_t *process)
{
	int fd;
	char buf[64] = { 0 };
	ssize_t chr_count = 0;
	ssize_t cmd_len = 0;

	if (!process)
		return chr_count;

	{
		snprintf(buf, ARRLEN(buf) - 1,
				"/proc/%d/cmdline", process->id);
		
		fd = open(buf, O_RDONLY);
		if (fd == -1)
			return chr_count;
	}

	while ((chr_count = read(fd, buf, sizeof(buf)) / sizeof(char))) {
		char *old_cmdline = process->cmdline;

		process->cmdline = (char *)calloc(
			cmd_len + (chr_count / sizeof(char)) + 1,
			sizeof(char)
		);

		if (cmd_len && old_cmdline) {
			if (process->cmdline) {
				strncpy(process->cmdline,
					old_cmdline,
					cmd_len);
				
				free(old_cmdline);
			}
		}

		if (!process->cmdline)
			break;

		strncpy(&process->cmdline[cmd_len], buf, chr_count);
		cmd_len += chr_count / sizeof(char);

		process->cmdline[cmd_len] = '\x00';
	}

	close(fd);

	return cmd_len;
}

static int read_status(process_t *process)
{
	int ret = 0;
	char *status_file;
	char *ptr;

	if (!process)
		return ret;

	{
		int fd;
		char buf[255] = { 0 };
		ssize_t chr_count = 0;
		ssize_t status_len = 0;

		snprintf(buf, ARRLEN(buf) - 1,
			 "/proc/%d/status", process->id);
		
		fd = open(buf, O_RDONLY);

		if (fd == -1)
			return ret;
		
		while ((chr_count = read(fd, buf,
					 sizeof(buf)) / sizeof(char))) {
			char *old_status_file = status_file;

			status_file = (char *)calloc(
				status_len + chr_count + 1,
				sizeof(char)
			);

			if (status_len && old_status_file) {
				if (status_file) {
					strncpy(status_file,
						old_status_file,
						status_len);
					
					free(old_status_file);
				}
			}

			if (!status_file)
				break;

			strncpy(&status_file[status_len], buf, chr_count);

			status_len += chr_count / sizeof(char);

			status_file[status_len] = '\x00';
		}

		if (!status_file)
			return ret;
	}

	ptr = strstr(status_file, "State:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];

	process->state = *ptr;

	ptr = strstr(ptr, "PPid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	process->parent = (pid_t)atoi(ptr);

	ptr = strstr(ptr, "TracerPid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];
	process->tracer = (pid_t)atoi(ptr);

	ptr = strstr(ptr, "Uid:");
	ptr = strchr(ptr, '\t');
	ptr = &ptr[1];

	{
		uid_t uid = (uid_t)atoi(ptr);
		struct passwd *powner = getpwuid(uid);
		process->owner = *powner;
	}

	ret = !ret;
	free(status_file);
	return ret;
}

int open_process(pid_t pid, process_t *process)
{
	int ret = 0;

	if (!process)
		return ret;

	/* Set 'pid' */
	process->id = pid;

	/* Read 'exe_path' */
	if (!read_exe_path(process))
		goto L_EXE_PATH_RET;

	/* Read 'cmdline' */
	if (!read_cmdline(process)) {
		goto L_CMDLINE_RET;
	}

	/* Read 'parent', 'tracer', 'state', 'owner' */
	if (!read_status(process)) {
		goto L_STATUS_RET;
	}

	goto L_SUCCESS;

	/* Error labels */
	{
	L_STATUS_RET:
		free(process->cmdline);
	L_CMDLINE_RET:
		free(process->exe_path);
	L_EXE_PATH_RET:
		return ret;
	}

L_SUCCESS:
	ret = !ret;
	return ret;
}

void close_process(process_t *process)
{
	CHECK_FREE(process->exe_path);
	CHECK_FREE(process->cmdline);
}
