/***************************************************************/
/*                                                             */
/*                  RISCV-LC Assembler                         */
/*                                                             */
/*                     CEG3420 Lab2                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/


#include "util.h"


void _assert(const char* expr_str, bool expr, const char* file, int line, const char* msg) {
    if (!expr) {
    	error("Assertion failed: %s, expected: %s, source: %s, line: %d\n",
    		msg, expr_str, file, line
    	);
    	exit(EXIT_FAILURE);
    }
}


void test(const char *format, ...) {
	va_list marker;
	va_start(marker, format);
	printf("[TEST]: ");
	vprintf(format, marker);
	va_end(marker);
}


void info(const char *format, ...) {
	va_list marker;
	va_start(marker, format);
	printf("[INFO]: ");
	vprintf(format, marker);
	va_end(marker);
}


void warn(const char *format, ...) {
	va_list marker;
	va_start(marker, format);
	printf("[WARN]: ");
	vprintf(format, marker);
	va_end(marker);
}


void error(const char *format, ...) {
	va_list marker;
	va_start(marker, format);
	printf("[ERROR]: ");
	vprintf(format, marker);
	va_end(marker);
	exit(EXIT_FAILURE);
}


FILE * open(char *file_name, char *mode) {
	FILE * fp = fopen(file_name, mode);
	if (fp == NULL) {
		error("cannot open the file %s.\n", file_name);
	}
	return fp;
}


void close(FILE *fp) {
	fclose(fp);
}


bool cmp_str(const char *tgt, const char *src) {
	if (
		strncmp(src, tgt, strlen(tgt)) == 0 && \
		strlen(src) == strlen(tgt)
	)
		return true;
	else
		return false;
}

void copy_str(char *tgt, const char *src) {
	char *ptr;
	ptr = strncpy(tgt, src, strlen(src));
	ptr[strlen(src)] = '\0';
}
