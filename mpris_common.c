#include "mpris_common.h"
#include <stdarg.h>
#include <stdio.h>

/*
 * Debug
 */
void do_debug(const char *fmt, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    printf("\e[32m\e[1mMPRIS Debug Info: \e[0m\e[34m");
    vprintf(fmt, arg_ptr);
    printf("\e[0m\n");
    va_end(arg_ptr);
}
