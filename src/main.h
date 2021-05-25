#ifndef HEADERS_H
#define HEADERS_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/ptrace.h>

#define ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))

#endif
