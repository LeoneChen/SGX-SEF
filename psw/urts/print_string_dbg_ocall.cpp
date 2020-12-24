//
// Created by leone on 2020/10/14.
//

#include <stdio.h>

extern "C" void print_string_dbg_ocall(const char *str) {
    printf("%s", str);
}
