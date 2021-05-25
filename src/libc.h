#ifndef LIBC_H
#define LIBC_H

#include "main.h"
#include "process.h"
#include "arch.h"

typedef struct libc_info_t {
	int   arch;
	char *so_path;
	void *dlopen_addr;
	void *dlclose_addr;
	void *dlsym_addr;
} libc_info_t;

int open_libc(process_t *process, libc_info_t *libc_info);
int close_libc(libc_info_t *libc_info);

#endif
