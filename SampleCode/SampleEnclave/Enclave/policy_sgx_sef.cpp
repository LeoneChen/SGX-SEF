//
// Created by leone on 2021/4/28.
//
#include "check_point.hpp"
#include "Enclave_t.h"

static void *call_seq[] = {
        (void *) ecall_array_in,
        (void *) ecall_array_out,
        (void *) ecall_array_in_out,
        (void *) ecall_array_isary,
        (void *) ecall_pointer_user_check,
        (void *) ecall_pointer_in,
        (void *) ocall_print_string,
};
static std::vector <cp_info_t> cp_call_seq;
static bool is_initialized = false;
static pthread_rwlock_t policy_rwlock = PTHREAD_RWLOCK_INITIALIZER;

bool cp_info_filter(cp_info_t info) {
    return (info.interface_type == INTERFACE_ECALL or info.interface_type == INTERFACE_OCALL);
}

// Exposed API for policy check logic, must be set for sgx-sef, can default return true.
// Has applied log_rwlock_rdlock in wrapper
bool CheckPoint::policy_check_user(cp_info_t info, std::deque <cp_info_t> log) {
    if (not is_initialized) {
        pthread_rwlock_wrlock(&policy_rwlock);
        for (size_t i = 0; i < 7; i++) {
            cp_info_t info;
            if (not get_func_info(call_seq[i], &info)) {
                pthread_rwlock_unlock(&policy_rwlock);
                return false;
            }
            cp_call_seq.push_back(info);
        }
        is_initialized = true;
        pthread_rwlock_unlock(&policy_rwlock);
    }

    for (auto it = cp_call_seq.rbegin(); it != cp_call_seq.rend(); it++) {
        if (cp_info_filter(*it) and _is_info_equal(info, *it)) {
            it++;
            for (auto log_it = log.rbegin();
                 log_it != log.rend() and it != cp_call_seq.rend(); log_it++, it++) {
                while (not cp_info_filter(*log_it)) {
                    log_it++;
                }
                if (not _is_info_equal(*log_it, *it)) {
                    return false;
                }
            }
            return true;
        }
    }
    return true;
}