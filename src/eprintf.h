#ifndef EPRINTF_H
#define EPRINTF_H

#include <stdarg.h>
#include <stdlib.h>

/*
 * print into stderr. eprintf runs exit(2) at the end. if last character of
 * fmt is ':', strerror(errno) will also be printed. always prints \n at
 * the end.
 */
void vweprintf(const char *fmt, va_list);
void weprintf(const char *fmt, ...);
void eprintf(const char *fmt, ...);

/*
 * run eprintf and stop the program if error happened.
 */
void *emalloc(size_t);
void *erealloc(void *, size_t);
char *estrdup(const char *);

#endif
