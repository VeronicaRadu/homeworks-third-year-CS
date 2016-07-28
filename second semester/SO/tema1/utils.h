/*
 * utils.h
 *
 *  Created on: Mar 7, 2016
 *      Author: arotaru
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string.h>
#include <stdlib.h>

#define ERROR_SUCCESS 0
#define ERROR_MISSING_PARAMETER 1
#define ERROR_UNKNOWN_COMMAND 2
#define ERROR_FAILURE 3

#define LINE_MAX_LENGTH 20000

#define ADD "add"
#define REMOVE "remove"
#define FIND "find"
#define CLEAR "clear"
#define PRINT_BUCKET "print_bucket"
#define PRINT "print"
#define RESIZE "resize"

#define UNUSED(x) (void)(x)

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while (0)


#define debu
#define trace_stream stderr
#ifdef debug

#define TRACE(msg, format, ...)										\
	do {															\
		fprintf(trace_stream, "[TRACE %s][%s at %d] [called %s(",	\
				msg, __FILE__, __LINE__, __FUNCTION__);				\
		fprintf(trace_stream, format, ##__VA_ARGS__);				\
		fprintf(trace_stream, ")]\n");								\
	} while (0)
#else
	#define TRACE(msg, format, ...)
#endif

typedef int (*comparer)(void *d1, size_t dl1, void *d2, size_t dl2);
typedef unsigned int uint;

typedef enum {
	add = 0,
	remov,
	find,
	clear,
	print_bucket,
	print,
	resize,
	unknown
} command;

/*0 = equal, else = not equal*/
int cmprer(void *d1, size_t dl1, void *d2, size_t dl2);
char *roty_strdup(const char *initial);
void correct_buffer(char *b);
void release_string(char **str);

#endif /* UTILS_H_ */
