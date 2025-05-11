/**
 * @file err.c
 * @brief Implementation of fatal and system error functions.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"

/* The code below is copied from one of the files shared as a part of the Computer
 * Network classes and has been created by teachers from the University of Warsaw. */

/**
 * Handles system or library errors (e.g., socket, bind failures).
 * Prints an error message including errno and its description,
 * then terminates the program with exit code 1.
 *
 * @param fmt Format string for a custom error message.
 * @param ... Additional arguments for formatting.
 */
noreturn void syserr(const char* fmt, ...) {
    va_list fmt_args;
    int org_errno = errno;

    fprintf(stderr, "\tERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);

    fprintf(stderr, " (%d; %s)\n", org_errno, strerror(org_errno));
    exit(1);
}

/**
 * Handles fatal errors that are not related to system calls
 * (e.g., wrong command-line parameters, invalid arguments).
 * Prints the error message and terminates the program with exit code 1.
 *
 * @param fmt Format string for a custom error message.
 * @param ... Additional arguments for formatting.
 */
noreturn void fatal(const char* fmt, ...) {
    va_list fmt_args;

    fprintf(stderr, "\tERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);

    fprintf(stderr, "\n");
    exit(1);
}
