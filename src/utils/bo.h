//
// Created by root on 8/18/24.
//

#ifndef EBPF_LLVM_JIT_BO_H
#define EBPF_LLVM_JIT_BO_H

#include <cstddef>
#include <cstdint>

namespace ebpf_llvm_jit::utils {

    typedef uint64_t(*precompiled_ebpf_function)(void *mem, size_t mem_len);

    static inline std::string ext_func_sym(uint32_t idx)
    {
        char buf[32];
        sprintf(buf, "_bpf_helper_ext_%04" PRIu32, idx);
        return buf;
    }
}

#endif //EBPF_LLVM_JIT_BO_H
