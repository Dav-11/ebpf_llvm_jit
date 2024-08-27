//
// Created by root on 8/27/24.
//

#include "helper.h"

#include <utility>

int ebpf_llvm_jit::helpers::helper::addToVm(ebpf_llvm_jit::jit::vm *vm) {
    return vm->register_external_function(index, name, fn);
}

ebpf_llvm_jit::helpers::helper::helper(unsigned int index, std::string name, void *fn) {

    this->fn = fn;
    this->name = std::move(name);
    this->index = index;

}

unsigned int ebpf_llvm_jit::helpers::helper::getIndex() const {
    return index;
}

const std::string &ebpf_llvm_jit::helpers::helper::getName() const {
    return name;
}

void *ebpf_llvm_jit::helpers::helper::getFn() const {
    return fn;
}

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