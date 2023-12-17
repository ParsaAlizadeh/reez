#ifndef EPRINTF_H
#define EPRINTF_H

#include <stdarg.h>
#include <stdlib.h>

extern void setprogname(const char *);
extern const char *getprogname(void);

/*
 * print into stderr. eprintf runs exit(2) at the end. if last character of
 * fmt is ':', strerror(errno) will also be printed. always prints \n at
 * the end.
 */
extern void vweprintf(const char *fmt, va_list);
extern void weprintf(const char *fmt, ...);
extern void eprintf(const char *fmt, ...);

/*
 * on error, run eprintf and stop the program.
 */
extern void *emalloc(size_t);
extern void *erealloc(void *, size_t);
extern char *estrdup(const char *);

#endif
