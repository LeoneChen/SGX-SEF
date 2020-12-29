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
#include "sgx_thread.h" // for sgx_thread_mutex_t
#include <deque> // for std::deque

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
    void *ocall_func;
    char *ocall_name;
    char *ubridge_name;
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
    void *ecall_func;
    char *ecall_name;
    char *tbridge_name;
} ecall_extra_info_t;
#endif //ECALL_EXTRA_INFO_T

#ifndef ECALL_EXTRA_TABLE_T
#define ECALL_EXTRA_TABLE_T
typedef struct {
    size_t nr_ecall;
    ecall_extra_info_t table[];
} ecall_extra_table_t;
#endif //ECALL_EXTRA_TABLE_T

#ifndef CP_INFO_T
#define CP_INFO_T
typedef struct cp_info {
    interface_type_t interface_type;
    int func_index;
    void *ms;
} cp_info_t;
#endif //CP_INFO_T

#ifndef CP_POLICY_ENTRY_T
#define CP_POLICY_ENTRY_T
typedef struct cp_policy_entry {
    cp_info_t info;
} cp_policy_entry_t;
#endif //CP_POLICY_ENTRY_T

#ifndef POLICY_ENTRY_T
#define POLICY_ENTRY_T
typedef struct policy_entry {
    void *func_addr;
} policy_entry_t;
#endif //POLICY_ENTRY_T

#ifndef CP_POLICY_T
#define CP_POLICY_T
typedef std::vector <cp_policy_entry_t> cp_policy_t;
#endif //CP_POLICY_T


#ifndef CLASS_CHECK_POINT
#define CLASS_CHECK_POINT

class CheckPoint {
public:
    int trigger(interface_type_t interface_type, int func_index, void *ms);

    void show_log(std::string title = "CP_INFO");

    CheckPoint();

    ~CheckPoint(){};// Class's Destructor Function in Enclave is never triggered

private:
    int _trigger(cp_info_t info);

    bool default_init_policy();

    bool policy_check(cp_info_t info);

    bool default_policy_check(cp_info_t info, cp_policy_t policy, std::deque <cp_info_t> log);

    bool default_policy_filter(cp_info_t info);

    void log(cp_info_t info);

    void _log_file_mod(cp_info_t info);

    void _show_log_file_mode(std::string title = "CP_INFO");

    cp_info_t str2info(char *str, const char *limiter);

    /* provide cp_info_t that contains Index and Interface Type,
     * but the latter only can tell you it's a ecall or ocall,
     * the ms info is set to nullptr.
     */
    bool get_func_info(void *addr, cp_info_t *info);

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

    sgx_thread_mutex_t m_log_mutex = SGX_THREAD_MUTEX_INITIALIZER;
    std::deque <cp_info_t> m_log;
    cp_policy_t m_policy;
    bool m_policy_inititalized = false;
    size_t m_policy_size = 0;

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
extern size_t g_policy_size;
extern policy_entry_t g_policy[];

#endif //CHECK_POINT_HPP