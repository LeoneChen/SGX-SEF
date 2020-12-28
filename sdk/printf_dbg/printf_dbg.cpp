//
// Created by leone on 2020/10/14.
//

#ifndef DEBUG
#define DEBUG
#endif

#include <printf_dbg.h> // Contain stdarg.h for va_list and extern "C" declaration

#ifdef DEBUG
#include <string.h> // for strlen
#include <stdio.h> // for vsnprintf, BUFSIZ
#include "sgx_printf_dbg_t.h" // for print_string_dbg_ocall
#endif //DEBUG

int printf_dbg(const char *fmt, ...) {
#ifndef DEBUG
    (void) fmt;
    return 0; // If not dbg mode, then do nothing.
#else //DEBUG
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    print_string_dbg_ocall(buf);
    return (int) strnlen(buf, BUFSIZ - 1) + 1;
#endif //DEBUG
}
