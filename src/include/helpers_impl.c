//
// Created by root on 8/27/24.
//

#include "helpers_impl.h"

// bpf_printk
uint64_t _bpf_helper_ext_0006(uint64_t fmt, uint64_t fmt_size, ...)
{
    const char *fmt_str = (const char *)fmt;
    va_list args;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wvarargs"
    va_start(args, fmt_str);
    long ret = vprintf(fmt_str, args);
#pragma GCC diagnostic pop
    va_end(args);
    return 0;
}
