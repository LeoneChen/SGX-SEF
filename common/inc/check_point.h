//
// Created by leone on 2020/12/14.
//

#ifndef CHECK_POINT_H
#define CHECK_POINT_H

#include <stddef.h> // for size_t
//#include <stdbool.h>
#ifndef INTERFACE_TYPE_T
#define INTERFACE_TYPE_T
typedef enum interface_type {
    INTERFACE_ENTER_ENCLAVE = 0x00,
    INTERFACE_EXIT_ENCLAVE = 0x01,
    INTERFACE_ECALL = 0x02,
    INTERFACE_ECALL_RET = 0x03,
    INTERFACE_SL_ECALL = 0x04,
    INTERFACE_SL_ECALL_RET = 0x05,
    INTERFACE_OCALL = 0x06,
    INTERFACE_OCALL_RET = 0x07,
    INTERFACE_SL_OCALL = 0x08,
    INTERFACE_SL_OCALL_RET = 0x09,
    INTERFACE_SL_OCALL_FALLBACK = 0x0a,
    INTERFACE_AEX = 0x0b,
    INTERFACE_AEX_FIN = 0x0c,
} interface_type_t;
#endif //INTERFACE_TYPE_T

#ifndef TRTS_OCALL_EXTRA_INFO_T
#define TRTS_OCALL_EXTRA_INFO_T
typedef struct {
    void* ocall_func;
    char* ocall_name;
    char* ubridge_name;
} trts_ocall_extra_info_t;
#endif //TRTS_OCALL_EXTRA_INFO_T

#ifndef TRTS_OCALL_EXTRA_TABLE_T
#define TRTS_OCALL_EXTRA_TABLE_T
typedef struct {
    size_t nr_ocall;
    trts_ocall_extra_info_t table[];
} trts_ocall_extra_table_t;
#endif //TRTS_OCALL_EXTRA_TABLE_T

#ifndef ECALL_EXTRA_INFO_T
#define ECALL_EXTRA_INFO_T
typedef struct {
    void* ecall_func;
    char* ecall_name;
    char* tbridge_name;
} ecall_extra_info_t;
#endif //ECALL_EXTRA_INFO_T

#ifndef ECALL_EXTRA_TABLE_T
#define ECALL_EXTRA_TABLE_T
typedef struct {
    size_t nr_ecall;
    ecall_extra_info_t table[];
} ecall_extra_table_t;
#endif //ECALL_EXTRA_TABLE_T

#ifdef __cplusplus
extern "C" {
#endif

int g_check_point_trigger(interface_type_t interface_type, int func_index, void *ms);

#ifdef __cplusplus
}
#endif

#endif //CHECK_POINT_H