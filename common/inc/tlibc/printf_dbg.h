//
// Created by leone on 2020/10/14.
//

#ifndef _PRINTF_DBG_H_
#define _PRINTF_DBG_H_

#include <sgx_defs.h> // for SGXAPI
#include <stdarg.h> // for va_list

#ifdef __cplusplus
extern "C" {
#endif

int SGXAPI printf_dbg(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif  //_PRINTF_DBG_H_

