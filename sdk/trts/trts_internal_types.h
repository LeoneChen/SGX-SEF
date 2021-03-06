/*
 * Copyright (C) 2011-2020 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef TRTS_INTERNAL_TYPES_H
#define TRTS_INTERNAL_TYPES_H

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

typedef struct {
    const void     *ecall_addr;
    uint8_t        is_priv;
    uint8_t        is_switchless;
} ecall_addr_t;


typedef struct {
    size_t          nr_ecall;
    ecall_addr_t    ecall_table[1];
} ecall_table_t;


typedef struct {
    size_t  nr_ocall;
    uint8_t entry_table[1];
} entry_table_t;

#endif
