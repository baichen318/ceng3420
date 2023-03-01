/***************************************************************/
/*                                                             */
/*                  RISCV-LC Simulator                         */
/*                                                             */
/*                     CEG3420 Lab2                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/


#ifndef __UTIL_H__
#define __UTIL_H__


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef NDEBUG
#undef assert
#   define assert(expr, msg) \
    _assert(#expr, expr, __FILE__, __LINE__, msg)
#else
#include <assert.h>
#   define assert(expr, msg)
#endif

/*
 * assert function
 */
void _assert(const char*, bool, const char*, int, const char*);


/*
 * print information at the 'test' level
 */
void test(const char *, ...);


/*
 * print information at the 'info' level
 */
void info(const char *, ...);


/*
 * print information at the 'warn' level
 */
void warn(const char *, ...);


/*
 * print information at the 'error' level and exit
 */
void error(const char *, ...);


/*
 * open the file with a mode
 */
FILE* open(char *, char *);


/*
 * close the file
 */
void close(FILE *fp);


/*
 * compare the string
 */
bool cmp_str(const char *, const char *);


/*
 * copy the string
 */
void copy_str(char *, const char *);

#endif
