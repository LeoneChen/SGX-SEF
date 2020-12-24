//
// Created by leone on 2020/12/14.
//

#include "check_point.hpp"
#include "check_point.h" // for extern "C" declaration
int g_check_point_trigger(interface_type_t interface_type, int func_index, void *ms){
    return g_check_point->trigger(interface_type, func_index, ms);
}
