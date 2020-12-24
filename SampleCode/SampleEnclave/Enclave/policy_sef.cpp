//
// Created by leone on 2021/4/28.
//
#include "check_point.hpp"
#include "Enclave_t.h" // for ecall symbols
#include <printf_dbg.h>

static void *call_seq[] = {
        (void *) ecall_array_in,
        (void *) ecall_array_out,
        (void *) ecall_array_in_out,
        (void *) ecall_array_isary,
        (void *) ecall_pointer_user_check,
        (void *) ecall_pointer_in,
        (void *) ocall_print_string,
};

static size_t call_seq_size = 7;
static std::vector <cp_info_t> cp_call_seq;
static pthread_once_t once = PTHREAD_ONCE_INIT;

bool cp_info_filter(cp_info_t info) {
    return (info.interface_type == INTERFACE_ECALL or info.interface_type == INTERFACE_OCALL);
}

void init_once() {
    for (size_t i = 0; i < call_seq_size; i++) {
        cp_info_t info;
        if (not g_check_point->get_func_info(call_seq[i], &info)) {
            return;
        }
        cp_call_seq.push_back(info);
    }
}

// Exposed API for policy check logic, must be set for sgx-sef, can default return true.
// Has applied log_rwlock_rdlock in wrapper
bool policy_check_user(cp_info_t info, std::deque <cp_info_t> log) {
    pthread_once(&once,init_once);

    for (auto it = cp_call_seq.rbegin(); it != cp_call_seq.rend(); it++) {
        if (cp_info_filter(*it) and g_check_point->_is_info_equal(info, *it)) {
            it++;
            for (auto log_it = log.rbegin();
                 log_it != log.rend() and it != cp_call_seq.rend(); log_it++, it++) {
                while (not cp_info_filter(*log_it)) {
                    log_it++;
                }
                if (not g_check_point->_is_info_equal(*log_it, *it)) {
                    return false;
                }
            }
            return true;
        }
    }
    return true;
}