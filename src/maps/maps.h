#ifndef MAPS_H
#define MAPS_H

#include <sys/types.h>

struct module_t {
	void  *base;
	void  *end;
};

int enum_modules(pid_t pid,
		 int(*callback)(struct module_t *mod, char *path, void *arg),
		 void *arg);
int find_module(pid_t pid, char *match, struct module_t *mod);
int get_module(pid_t pid, char *name, struct module_t *mod);
size_t get_module_path(pid_t pid, struct module_t *mod,
		       char *path, size_t maxlen);


#endif
