#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "maps.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

typedef struct get_module_path_t {
	void  *base;
	char  *path;
	size_t len;
} get_module_path_t;

typedef struct get_module_t {
	char *name;
	struct module_t *mod;
} get_module_t;

typedef struct find_module_t {
	char *match;
	struct module_t *mod;
} find_module_t;

#define strtop strtoul

int enum_modules(pid_t pid,
		 int(*callback)(struct module_t *mod, char *path, void *arg),
		 void *arg)
{
	int ret = 0;
	char *maps = (char *)NULL;
	char *ptr;

	{
		int  fd;
		char buf[512] = { 0 };
		ssize_t size;
		ssize_t total = 0;

		snprintf(buf, sizeof(buf), "/proc/%d/maps", pid);

		fd = open(buf, O_RDONLY);
		if (fd == -1)
			return ret;

		while ((size = read(fd, (void *)buf, sizeof(buf)))) {
			char *old_maps = maps;
			size /= sizeof(char);
			maps = calloc(total + size + 1, sizeof(char));

			if (total) {
				if (maps)
					strncpy(maps, old_maps, total);
				free(old_maps);
			}

			strncpy(&maps[total], buf, size);

			total += size;
		}
		
		if (total)
			maps[total] = '\x00';

		close(fd);
	}

	if (!maps)
		return ret;

	ptr = maps;
	while ((ptr = strchr(ptr, '/'))) {
		int    cbret;
		char  *path;
		char  *tmp;
		char  *base;
		char  *end;
		size_t len;
		struct module_t mod;

		for (len = 0; ptr[len] != '\n'; ++len);

		path = calloc(len + 1, sizeof(char));
		if (!path) {
			ret = 0;
			break;
		}

		strncpy(path, ptr, len);
		path[len] = '\x00';

		for (tmp = base = maps;
		     (ptrdiff_t)(tmp = strchr(tmp, '\n')) < (ptrdiff_t)ptr;
		     tmp = &tmp[1]) {
			base = tmp;
		}

		if (*base == '\n')
			base = &base[1];

		for (tmp = maps; (tmp = strstr(tmp, path)); tmp = &tmp[1])
			ptr = tmp;
		
		for (tmp = maps;
		     (ptrdiff_t)(tmp = strchr(tmp, '\n')) < (ptrdiff_t)ptr;
		     tmp = &tmp[1]) {
			end = tmp;
		}

		end = strchr(end, '-');
		end = &end[1];

		mod.base = (void *)strtop(base, NULL, 16);
		mod.end  = (void *)strtop(end, NULL, 16);

		cbret = callback(&mod, path, arg);

		free(path);

		if (!cbret) {
			ret = !ret;
			break;
		}

		ptr = &ptr[len + 1];
	}

	free(maps);

	return ret;
}

static int find_module_cb(struct module_t *mod, char *path, void *arg)
{
	find_module_t *parg = (find_module_t *)arg;
	char *ptr;
	char *tmp;
	size_t match_len;
	size_t len;

	if (!parg || !parg->mod || !parg->match)
		return 0;

	for (tmp = ptr = path; (tmp = strchr(tmp, '/')); tmp = &tmp[1])
		ptr = &tmp[1];

	match_len = strlen(parg->match);
	len = strlen(ptr);
	if (len > match_len)
		len = match_len;

	if (!strncmp(ptr, parg->match, len)) {
		*parg->mod = *mod;
		return 0;
	}

	return 1;
}

int find_module(pid_t pid, char *match, struct module_t *mod)
{
	find_module_t arg;
	arg.match = match;
	arg.mod = mod;
	return enum_modules(pid, find_module_cb, (void *)&arg);
}

static int get_module_cb(struct module_t *mod, char *path, void *arg)
{
	get_module_t *parg = (get_module_t *)arg;
	size_t path_len;
	size_t name_len;

	if (!parg || !parg->mod || !parg->name)
		return 0;
	
	path_len = strlen(path);
	name_len = strlen(parg->name);

	if (name_len > path_len)
		return 1;
	
	if (!strcmp(&path[path_len - name_len], parg->name)) {
		*parg->mod = *mod;
		return 0;
	}

	return 1;
}

int get_module(pid_t pid, char *name, struct module_t *mod)
{
	get_module_t arg;
	arg.name = name;
	arg.mod  = mod;
	return enum_modules(pid, get_module_cb, (void *)&arg);
}

static int get_module_path_cb(struct module_t *mod, char *path, void *arg)
{
	get_module_path_t *parg = (get_module_path_t *)arg;
	if (!parg || !parg->len) {
		parg->len = 0;
		return 0;
	}

	if (mod->base == parg->base) {
		size_t path_len;

		path_len = strlen(path);

		if (path_len >= parg->len)
			path_len = parg->len - 1;

		strncpy(parg->path, path, path_len);
		parg->path[path_len] = '\x00';
		parg->len = path_len;

		return 0;
	}

	return 1;
}

size_t get_module_path(pid_t pid, struct module_t *mod,
		       char *path, size_t maxlen)
{
	get_module_path_t arg;
	arg.path = path;
	arg.len  = maxlen;
	arg.base = mod->base;
	enum_modules(pid, get_module_path_cb, (void *)&arg);
	return arg.len;
}
