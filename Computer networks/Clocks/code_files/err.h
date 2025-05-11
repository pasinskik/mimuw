/**
 * @file err.h
 * @brief Error reporting and system error handling.
 */


#ifndef MIM_ERR_H
#define MIM_ERR_H

#include <stdnoreturn.h>

/* Print information about a system error and quits. */
noreturn void syserr(const char* fmt, ...);

/* Print information about an error and quits. */
noreturn void fatal(const char* fmt, ...);

#endif
