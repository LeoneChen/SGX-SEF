//
// Created by leone on 2020/12/14.
//
#include <string.h> // for strtok
#include <stdarg.h> // for va_list
#include <rts_cmd.h> // for ECMD
#include <errno.h> // for errno
//#include <stdint.h> // for uint32_t
#include <printf_dbg.h>
#include <check_point.hpp>

//#define FILE_MODE// take too much time, an always-on file descriptor seem can't log all info correctly
//#define WARNING
//#define CLOSE_CP
#define UNUSED(val) (void)(val)

static CheckPoint _check_point;
CheckPoint *g_check_point = &_check_point;
//it seems thread local storage(thread_local, __thread) in some position can't work well

CheckPoint::CheckPoint() {
    pthread_rwlock_init(&m_log_rwlock, NULL);

#ifdef FILE_MODE
    int try_time = 10;
    while (sgx_remove("/home/leone/check_point.log") != 0 and errno == EAGAIN and
           try_time--); // remove log file firstly
#endif
}

int CheckPoint::trigger(interface_type_t interface_type, int func_index, void *ms) {
#ifdef CLOSE_CP
    UNUSED(interface_type);
    UNUSED(func_index);
    UNUSED(ms);
    return 1;
#else
    cp_info_t info = {interface_type, func_index, ms};
    return _trigger(info);
#endif
}

int CheckPoint::_trigger(cp_info_t info) {
    if ((info.interface_type == INTERFACE_OCALL or info.interface_type == INTERFACE_OCALL_RET)
        and (_is_ignored_ocall(info)))
        return 1;// in case of nested ocall

    if (policy_check(info)) {
        log(info);
        return 1;// 1 means check ok
    }
#ifdef WARNING
    log(info);
    return -1;// -1 means just warning
#else
    return 0;// 0 means error
#endif
}

bool CheckPoint::policy_check(cp_info_t info) {
#ifdef FILE_MODE
    UNUSED(info);
    return true;// just for try
#else // MEM_MODE
    bool ret = false;
    pthread_rwlock_rdlock(&m_log_rwlock);
    ret = policy_check_user(info, m_log);
    pthread_rwlock_unlock(&m_log_rwlock);
    return ret;
#endif // FILE_MODE
}

bool CheckPoint::_is_info_equal(cp_info_t info1, cp_info_t info2) {
    return (info1.interface_type == info2.interface_type and info1.func_index == info2.func_index)
           ? true : false;
}

void CheckPoint::log(cp_info_t info) {
    pthread_rwlock_wrlock(&m_log_rwlock);

#ifdef FILE_MODE
    _log_file_mod(info);
#else //MEM_MODE
    while (m_log.size() >= 100) { //epc size is limited
        m_log.pop_front();
    }
    m_log.push_back(info);
#endif //FILE_MODE

    pthread_rwlock_unlock(&m_log_rwlock);
}

void CheckPoint::_log_file_mod(cp_info_t info) {
    SGX_FILE *log_fp = nullptr;
    int try_time = 10;
    do {
        log_fp = sgx_fopen_auto_key("/home/leone/check_point.log", "a+");
    } while (log_fp == nullptr and errno == EAGAIN and try_time--);
    if (log_fp == nullptr) return;

    sgx_fseek(log_fp, 0L, SEEK_END);
    std::string str = std::to_string(info.interface_type) + " " + std::to_string(info.func_index) + " " +
                      std::to_string((uint64_t) info.ms) + "\n";
    sgx_fwrite(str.c_str(), sizeof(char), str.size(), log_fp);

    try_time = 10;
    while (sgx_fclose(log_fp) != 0 and errno == EAGAIN and try_time--);
}

void CheckPoint::show_log(std::string title, bool (*filter)(cp_info_t)) {
    pthread_rwlock_rdlock(&m_log_rwlock);

#ifdef FILE_MODE
    _show_log_file_mode(title,filter);
#else //MEM_MODE
    printf_dbg("================LOG MEM MODE================\n");

    for (auto info:m_log)
        if (filter(info))
            _show_info(info, title);


    printf_dbg("================LOG MEM MODE END================\n");
#endif //FILE_MODE
    pthread_rwlock_unlock(&m_log_rwlock);
}

void CheckPoint::_show_log_file_mode(std::string title, bool (*filter)(cp_info_t)) {
    printf_dbg("================LOG FILE MODE================\n");

    SGX_FILE *log_fp = nullptr;
    int try_time = 10;
    do {
        log_fp = sgx_fopen_auto_key("/home/leone/check_point.log", "r");
    } while (log_fp == nullptr and errno == EAGAIN and try_time--);
    if (log_fp == nullptr) return;

    sgx_fseek(log_fp, 0L, SEEK_SET);
    char line[BUFSIZ];
    do {
        if (sgx_freadline(line, BUFSIZ, log_fp) == 0) break;
        auto info = str2info(line, " ,");
        if (filter(info))
            this->_show_info(info, title);
    } while (true);

    try_time = 10;
    while (sgx_fclose(log_fp) != 0 and errno == EAGAIN and try_time--);

    printf_dbg("================LOG FILE MODE END================\n");
}

cp_info_t CheckPoint::str2info(char *str, const char *delimiter) {
    std::vector <std::string> strs;
    char *p = strtok(str, delimiter);
    while (p != NULL) {
        strs.push_back(std::string(p));
        p = strtok(NULL, delimiter);
    }
    return {(interface_type_t) std::stoi(strs[0], nullptr, 10), std::stoi(strs[1], nullptr, 10),
            reinterpret_cast<void *>(std::stoull(strs[2], nullptr, 10))};
}

/* provide cp_info_t that contains Index and Interface Type,
 * but the latter only can tell you it's a ecall or ocall,
 * the ms info is set to nullptr.
 */
bool CheckPoint::get_func_info(void *addr, cp_info_t *info) {
    bool is_ecall;
    int idx = get_func_idx(addr, &is_ecall);
    if (idx == -1) return false;

    info->func_index = idx;
    info->interface_type = is_ecall ? INTERFACE_ECALL : INTERFACE_OCALL;
    info->ms = nullptr;
    return true;
}

int CheckPoint::get_func_idx(void *addr, bool *is_ecall) {
    if (addr == nullptr) return false;
    int idx = _get_func_idx_in_etable(addr);
    if (idx != -1) {
        *is_ecall = true;
        return idx;
    }
    idx = _get_func_idx_in_otable(addr);
    if (idx != -1)
        *is_ecall = false;
    return idx;
}

int CheckPoint::_get_func_idx_in_etable(void *addr) {
    for (int i = 0; i < (int) g_ecall_extra_table.nr_ecall; i++) {
        if (g_ecall_extra_table.table[i].ecall_func == addr) {
            return i;
        }
    }
    return -1;
}

int CheckPoint::_get_func_idx_in_otable(void *addr) {
    for (int i = 0; i < (int) g_trts_ocall_extra_table.nr_ocall; i++) {
        if (g_trts_ocall_extra_table.table[i].ocall_func == addr) {
            return i;
        }
    }
    return -1;
}

std::string CheckPoint::get_func_name(interface_type_t interface_type, int func_index, void *ms) {
    cp_info_t info = {interface_type, func_index, ms};
    return _get_func_name(info);
}

std::string CheckPoint::_get_func_name(cp_info_t info) {
    if (info.func_index < 0) {
        switch (info.func_index) {
            case ECMD_INIT_ENCLAVE:
                return "ECMD_INIT_ENCLAVE";
            case ECMD_ORET:
                return "ECMD_ORET";
            case ECMD_EXCEPT:
                return "ECMD_EXCEPT";
            case ECMD_MKTCS:
                return "ECMD_MKTCS";
            case ECMD_UNINIT_ENCLAVE:
                return "ECMD_UNINIT_ENCLAVE";
            case ECMD_ECALL_PTHREAD:
                return "ECMD_ECALL_PTHREAD";
            default:
                return "Unknown ECMD";
        }
    } else {
        switch (info.interface_type) {
            case INTERFACE_ECALL:
            case INTERFACE_ECALL_RET:
            case INTERFACE_SL_ECALL:
            case INTERFACE_SL_ECALL_RET:
                return std::string(g_ecall_extra_table.table[info.func_index].ecall_name);
            case INTERFACE_OCALL:
            case INTERFACE_OCALL_RET:
            case INTERFACE_SL_OCALL:
            case INTERFACE_SL_OCALL_RET:
            case INTERFACE_SL_OCALL_FALLBACK:
                return std::string(g_trts_ocall_extra_table.table[info.func_index].ocall_name);
            default:
                return "Unknown FUNC";
        }
    }

}

std::string CheckPoint::itype2str(interface_type_t interface_type) {
    switch (interface_type) {
        case INTERFACE_ENTER_ENCLAVE:
            return "INTERFACE_ENTER_ENCLAVE";
        case INTERFACE_EXIT_ENCLAVE:
            return "INTERFACE_EXIT_ENCLAVE";
        case INTERFACE_ECALL:
            return "INTERFACE_ECALL";
        case INTERFACE_ECALL_RET:
            return "INTERFACE_ECALL_RET";
        case INTERFACE_SL_ECALL:
            return "INTERFACE_SL_ECALL";
        case INTERFACE_SL_ECALL_RET:
            return "INTERFACE_SL_ECALL_RET";
        case INTERFACE_OCALL:
            return "INTERFACE_OCALL";
        case INTERFACE_OCALL_RET:
            return "INTERFACE_OCALL_RET";
        case INTERFACE_SL_OCALL:
            return "INTERFACE_SL_OCALL";
        case INTERFACE_SL_OCALL_RET:
            return "INTERFACE_SL_OCALL_RET";
        case INTERFACE_SL_OCALL_FALLBACK:
            return "INTERFACE_SL_OCALL_FALLBACK";
        case INTERFACE_AEX:
            return "INTERFACE_AEX";
        case INTERFACE_AEX_FIN:
            return "INTERFACE_AEX_FIN";
        default:
            return "Unknown INTERFACE_TYPE";
    }
}

void CheckPoint::show_info(interface_type_t interface_type, int func_index, void *ms, std::string title) {
    cp_info_t info = {interface_type, func_index, ms};
    _show_info(info, title);
}

void CheckPoint::_show_info(cp_info_t info, std::string title) {
    printf_dbg("[%s] InterfaceType: %s(%d), FuncName: %s(%d), FuncParam: %p\n", title.c_str(),
               itype2str(info.interface_type).c_str(), info.interface_type, _get_func_name(info).c_str(),
               info.func_index, info.ms);
}

bool CheckPoint::is_ignored_ocall(interface_type_t interface_type, int func_index, void *ms) {
    cp_info_t info = {interface_type, func_index, ms};
    return _is_ignored_ocall(info);
}

bool CheckPoint::_is_ignored_ocall(cp_info_t info) {
    std::string func_name = _get_func_name(info);
    if (m_ignored_ocall_list.find(func_name) != m_ignored_ocall_list.end()) return true;
    return false;
}
