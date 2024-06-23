// memleak_hook.h

#ifndef MEMLEAK_HOOK_H
#define MEMLEAK_HOOK_H
#define _GNU_SOURCE
#include <dlfcn.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void init_hook(void);
void *malloc(size_t size);
void free(void *ptr);

#endif