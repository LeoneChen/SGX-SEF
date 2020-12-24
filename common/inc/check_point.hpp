//
// Created by leone on 2020/12/14.
//
#ifndef CHECK_POINT_HPP
#define CHECK_POINT_HPP

#include <pthread.h> // for pthread_rwlock
#include <sgx_tprotected_fs.h> // for trusted file operations
#include <vector> // for std::vector
#include <stddef.h> // for size_t
//#include <trts_internal_types.h> // for struct ecall_extra_table_t etc.
#include <string> // for string
#include <unordered_set>
#include <deque> // for std::deque
#include "sgx_thread.h" // for sgx_thread_mutex_t
#include "check_point.h"

#ifndef CP_INFO_T
#define CP_INFO_T
typedef struct cp_info {
    interface_type_t interface_type;
    int func_index;
    void *ms;
} cp_info_t;
#endif //CP_INFO_T

bool policy_check_user(cp_info_t info, std::deque <cp_info_t> log);

#ifndef CLASS_CHECK_POINT
#define CLASS_CHECK_POINT

class CheckPoint {
public:
    int trigger(interface_type_t interface_type, int func_index, void *ms);

    void show_log(std::string title = "CP_INFO", bool (*filter)(cp_info_t)=[](cp_info_t){return true;});

    CheckPoint();

    ~CheckPoint(){};// Class's Destructor Function in Enclave is never triggered

    /* provide cp_info_t that contains Index and Interface Type,
     * but the latter only can tell you it's a ecall or ocall,
     * the ms info is set to nullptr.
     */
    bool get_func_info(void *addr, cp_info_t *info);

    int _trigger(cp_info_t info);

    bool policy_check(cp_info_t info);

    void log(cp_info_t info);

    void _log_file_mod(cp_info_t info);

    void _show_log_file_mode(std::string title = "CP_INFO", bool (*filter)(cp_info_t)=[](cp_info_t){return true;});

    cp_info_t str2info(char *str, const char *limiter);

    int get_func_idx(void *addr, bool *is_ecall);

    int _get_func_idx_in_etable(void *addr);

    int _get_func_idx_in_otable(void *addr);

    std::string get_func_name(interface_type_t interface_type, int func_index, void *ms);

    std::string _get_func_name(cp_info_t info);

    std::string itype2str(interface_type_t interface_type);

    void show_info(interface_type_t interface_type, int func_index, void *ms, std::string title = "CP_INFO");

    void _show_info(cp_info_t info, std::string title = "CP_INFO");

    bool is_ignored_ocall(interface_type_t interface_type, int func_index, void *ms);

    bool _is_ignored_ocall(cp_info_t info);

    bool _is_info_equal(cp_info_t info1, cp_info_t info2);

    pthread_rwlock_t m_log_rwlock;
    std::deque <cp_info_t> m_log;

    std::unordered_set <std::string> m_ignored_ocall_list{
            "print_string_dbg_ocall",
            "sgx_thread_wait_untrusted_event_ocall", "sgx_thread_set_untrusted_event_ocall",
            "sgx_thread_setwait_untrusted_events_ocall", "sgx_thread_set_multiple_untrusted_events_ocall",
            "u_sgxprotectedfs_exclusive_file_open", "u_sgxprotectedfs_check_if_file_exists",
            "u_sgxprotectedfs_fread_node", "u_sgxprotectedfs_fwrite_node", "u_sgxprotectedfs_fclose",
            "u_sgxprotectedfs_fflush", "u_sgxprotectedfs_remove", "u_sgxprotectedfs_recovery_file_open",
            "u_sgxprotectedfs_fwrite_recovery_node", "u_sgxprotectedfs_do_file_recovery"
    };
};

#endif //CLASS_CHECK_POINT

extern CheckPoint *g_check_point;
extern ecall_extra_table_t g_ecall_extra_table;
extern trts_ocall_extra_table_t g_trts_ocall_extra_table;

#endif //CHECK_POINT_HPP
