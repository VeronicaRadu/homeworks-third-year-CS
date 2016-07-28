/*
 * debug_help.h
 *
 *  Created on: Mar 15, 2016
 *      Author: arotaru
 */

#ifndef DEBUG_HELP_H_
#define DEBUG_HELP_H_

typedef unsigned int uint;
extern uint tabs;

#define tab "   "
#define debug
#define trace_stream stderr

#ifdef debu

#define TRACE(msg, format, args...)\
    do {\
        uint tab_cnt = tabs++;\
        while (tab_cnt--)\
            fprintf(trace_stream, tab);\
        fprintf(trace_stream,\
                "[TRACE %s][%s at %d] [called %s(",\
                msg,\
                __FILE__,\
                __LINE__,\
                __func__);\
        fprintf(trace_stream, format, ##args);\
        fprintf(trace_stream, ")]{\n");\
    } while (0)
#define REPORT(msg)\
    do {\
        uint tab_cnt = tabs - 1;\
        while (tab_cnt--)\
            fprintf(trace_stream, tab);\
        fprintf(trace_stream,\
                "[REPORT(%s)][%s at %d calling %s]\n",\
                msg,\
                __FILE__,\
                __LINE__,\
                __func__);\
    } while (0)
#define RETRACE()\
    do {\
        uint tab_cnt = tabs--;\
        while (tab_cnt--)\
            fprintf(trace_stream, tab);\
        fprintf(trace_stream, "}\n");\
    } while (0)

#else

#define TRACE(msg, format, args...) (void)(msg)
#define REPORT(msg) (void)(msg)
#define RETRACE() (void)(' ')

#endif


#define UNUSED(x) (void)(x)

int inverted_dup2(int nfd, int ofd)
{
    return dup2(ofd, nfd);
}
#define to(fd_new) fd_new)
#define redirect(fd_old) inverted_dup2(fd_old,


#endif /* DEBUG_HELP_H_ */
